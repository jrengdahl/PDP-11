.text

.globl  _main

.globl  startup

    jmp startup

    .space 508

startup:
    mov     $-020000,sp     / set init stack pointer

    mov     $_sbss,r1       / clear the BSS section
    mov     $_ebss,r0
    sub     r1,r0
    beq     1f
0:
    clrb    (r1)+
    sob     r0,0b
1:
    clr     r5              / for adb and longjmp/rollback ...
    jsr     pc,_main        / call main
    mov     r0,-(sp)        / pass main's return value to _exit ...
    jsr     pc,_exit

