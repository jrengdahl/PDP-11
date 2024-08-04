// boot.S
// 2024.08.02 Jonathan R. Engdahl

// This is a bootstrap loader for the Bear Metal MSCP solid state drive.
// It is based in part on the bootloader in SIMH, found in pdp11_rq.c.
// The following is the attribution for that source:

/* pdp11_rq.c: MSCP disk controller simulator

   Copyright (c) 2002-2013, Robert M Supnik
   Derived from work by Stephen F. Shirron

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   ROBERT M SUPNIK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Robert M Supnik shall not be
   used in advertising or otherwise to promote the sale, use or other dealings
   in this Software without prior written authorization from Robert M Supnik.
*/

// I also refered to other bootloaders such as those found at
// https://www.ibiblio.org/pub/academic/computer-science/history/pdp-11/bootstraps


// The original bootloader is stored in a ROM on an MSCP controller.
// When invoked, it does the following:
// -- initlialize communicaton with the controller
// -- bring the drive online
// -- copy the first 512-byte sector from the selected unit to address zero
// -- jump to address zero to start the second stage bootloader

// I cleaned up the code, figured out how it works, and added detailed comments
// that should be helpful, particularly in light of the fact that the MSCP protocol
// is rather opaque, and little at all is known about the lower layer protocol,
// UQSSP. The MSCP document is broad and non-specific, covering everything from a
// VAXcuster to an RQDX3 attached to a 20-megabyte ST-225. The RQDX3 implementation
// of MSCP is greatly simplified. Most of what I know about these protocols was
// gleaned from examining code written decades ago by those who had accss to the
// documents.


    .org 0173000

    // assume the PDP-11 has a minimum of 64K of memory. The first 32K will be
    // used for the boot program. The data area for the bootloader will start at 32K.

    base = 040000               // base address of memory shared between host and controller
    rsp_packet = base + 4       // reply buffer
    cmd_packet = base + 104     // command buffer
    comm_area = base + 204      // the reply and comand FIFOs

// Four step init process
start:
        mov     $160000,sp      // point the sp to somewhere we know there is RAM, and this program is not there
        mov     $unit,r0        // the disk unit number, default 0
        mov     $172150, r1     // address of the IP register
        mov     $table, r4      // address of the table of constants    
        mov     $4000,r5        // s1 mask, this will become s2, s3, and s4 as it is shifted left in the loop
        mov     r1,r2           // get the address of the SA register
        clr     (r2)+           // write to IP, resetting the controller, and increment r2 to point to SA

        // r0   unit number
        // r1   address of IP register
        // r2   address of SA register
        // r3   scratch
        // r4   pointer into table of constants
        // r5   step mask

        // the four-step init loop
10$:    mov     (r2), r3        // get SA register
        bpl     11$             // if the error bit is set, jump to halt
        halt                    // halt on error
11$:    bit     r5,r3           // test the SA value with the step mask to see if the controller has written the currently expected step to SA
        beq     10$             // if not the expected step yet, then loop, waiting for either an error or the next step
        mov     (r4)+,(r2)      // write the host response to the SA register
        asl     r5              // shift to get the mask for the next step
        bpl     10$             // loop until completion of step 4, as indicated by shifting of mask to bit 15

        clr     r3              / don't need bytecount size for online command

/* Send ONL, READ commands */

30$:    tstb    (r4)            // check for end of table of constants
        beq     done            // 0 = yes


        // loop to clear the packets, leaving r2 pointing to the comm area
        mov     $rsp_packet-4,r2 // clr pkts
40$:    clr     (r2)+
        cmp     r2,#comm_area
        blo     40$

        // r0   unit number
        // r1   address of IP register
        // r2   pointer to reply and command FIFOs (each with only one entry)
        // r3   unit size
        // r4   pointer into table of constants, pointing to the start of the command table
        // r5   contains 100000 (bit 15 set) from the final asl, which can be used to set the OWNER bit in packet descriptors 

        // setup an online or read command and reply
        // for the source LBN and the the buffer will both be zero
        movb    (r4)+,cpkt-4    // set command packet length from the table into the UQSSP header (-4)
        movb    r0,cpkt+4       // set the unit number in the command packet
        movb    (r4)+,cpkt+10   // set command opcode from the table
        mov     $rpkt,(r2)+     // save the address of the reply buffer as the low word of the reply descriptor
        mov     r5,(r2)+        // save r5, with the OWNER bit set, as the high word of the reply descriptor
        mov     $ckpt,(r2)+     // save the address of the command buffer as the low word of the reply descriptor
        mov     r5,(r2)         // save r5, with the OWNER bit set, as the high word of the command descriptor
        cmp     -(r2),-(r2)     // restore r2 back to start of FIFOs (decrement r2 by 4 (using a one-word instruction that only affects the condition)
        tst     (r1)            // wake ctrl by reading the IP register

50$:    tst     (r2)            // see if controller has returned the reply buffer to the host
        bmi     50$             // as long as the reply is still owned by te controller
        tst     rpkt+12         // check the reply status word
        beq     52$             // if status was nonzero then terminate         TODO fix this loop, should check for error continuously
        halt                    // halt on error
52$:    tst     rpkt+46         // if the unit size is larger than 64K blocks
        bne     55$             // go set the unit size to 64
        mov     rpkt+44,r3      // get the low word of the unit size
        cmp     r3,#40          // if <= 64 blocks
        ble     56$             // use the count as-is
55$:    mov     $40,r3          // else make it 64
56$:    ash     $11,r3          // convert to byte count (multiply by 512 bytes per block)

        jmp      $30             

// Boot block read in, jump to 0

done:   clr     (r1)            // re-initilize the controller (for M+, which I assume refers to RSX-11M+) 
        clr     r3              // some requirement for the second state boot?
        mov     $st+020,r4      // this is the address of the table of constants
        clr     r5              //

        // r0   unit number
        // r1   address of IP register, i.e. the boot device's CSR
        // r2   pointer to reply and command FIFOs
        // r3   0
        // r4   pointer to the table of constants
        // r5   0

        clr     pc              // jump to zero, the start of the second stage boot loader


 /* Data */


table:
        // table of four responses to the controller's prompts during the four-step init process
        .word   0100000         // no interrupts, no vector, ring size = 1, "valid" bit set
        .word   comm_area       // the address of the third word of the communication area
        .word   0000000         // the upper bits of the address of the communication area
        .word   0000001         // the GO bit, but don't bother with error packets

        // command table, contains constants for the commands required to load the next stage bootloader
        // online command
        .byte   20              // online packet length for the UQSSP header (apparently UQSSP can truncate bytes that are zero at the end of a packet)
        .byte   11              // online command

        // constant for the read command
        .byte   40              // read packet length for the UQSSP header (32 bytes, up though LBN)
        .byte   41              // read command

        // end of table flag, the first byte of all previous command table entries must be nonzero
        .word   0
