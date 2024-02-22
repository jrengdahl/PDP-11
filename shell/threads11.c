#include "threads11.h"


int *pending_chain = 0;

int wait(PORT *port __attribute__((__unused__)))
    {
    __asm__ __volatile__(
    "mov 2(sp),r0               \n\t"   // get the address of the port

    "mfps r1                    \n\t"   // save the low byte of the current PSW
    "spl 7                      \n\t"   // Set the processor priority level to 7, disabling all interrupts
    "mov r5,-(sp)               \n\t"   // push r5
    "mov r4,-(sp)               \n\t"   // push r4
    "mov r3,-(sp)               \n\t"   // push r3
    "mov r2,-(sp)               \n\t"   // push r2
    "mov @r0,-(sp)              \n\t"   // push the current thread at the port
    "mov sp,@r0                 \n\t"   // save this thread at the port

    "mov _pending_chain,sp      \n\t"   // get the top of the pending chain
    "mov (sp)+, _pending_chain  \n\t"   // unlink the next thread on the chain
    "mov (sp)+,r2               \n\t"   // restore r2
    "mov (sp)+,r3               \n\t"   // restore r3
    "mov (sp)+,r4               \n\t"   // restore r4
    "mov (sp)+,r5               \n\t"   // restore r5
    "mtps r1                    \n\t"   // Restore the low byte of the PSW from previous_state
    );

    return 1;                           // return true from the call to wake
    }

int wake(PORT *port __attribute__((__unused__)), int arg __attribute__((__unused__)))
    {
    __asm__ __volatile__(
    "mov 2(sp),r0               \n\t"   // get the first arg, the address of the port to wake
    "tst @r0                    \n\t"   // make sure there is a thread waiting to be waked
    "beq 0f                     \n\t"   // go return false from wake if not

    "mfps r1                    \n\t"   // save the low byte of the current PSW
    "spl 7                      \n\t"   // Set the processor priority level to 7, disabling all interrupts
    "mov r5,-(sp)               \n\t"   // push r5
    "mov 6(sp),r5               \n\t"   // get the second arg, a value to be returned from wait
    "mov r4,-(sp)               \n\t"   // push r4
    "mov r3,-(sp)               \n\t"   // push r3
    "mov r2,-(sp)               \n\t"   // push r2
    "mov _pending_chain,-(sp)   \n\t"   // push/link the current thread at the top of the pending chain
    "mov sp,_pending_chain      \n\t"   // save this thread as the new top of the pending chain

    "mov @r0,sp                 \n\t"   // get the first thread at the port
    "mov (sp)+,@r0              \n\t"   // unlink the next thread to the port
    "mov r5,r0                  \n\t"   // this is the value to be returned from wait
    "mov (sp)+,r2               \n\t"   // restore r2
    "mov (sp)+,r3               \n\t"   // restore r3
    "mov (sp)+,r4               \n\t"   // restore r4
    "mov (sp)+,r5               \n\t"   // restore r5
    "mtps r1                    \n\t"   // Restore the low byte of the PSW from previous_state
    "rts pc                     \n\t"   // return from wait with the value that was passed

"0:                             \n\t"
    );

    return 0;                           // nothing to wake -- return a false from wake
    }



// When spawning a thread up to four args can be passed to it.
// In C the number is not known by the compiler. The spawn routine will pass the first four args to the new stack.
// If the caller does not pass thay many the unused slots will be ignored. Just make sure the caller and the
// new thread agree on the number of args. You could not get away with this so easily in C++.
 

void spawn(
    THREADFUNCTION *code    __attribute__((__unused__)),
    int *stack_end          __attribute__((__unused__)),
    ...)
    {
    __asm__ __volatile__(
    "mov sp,r0                  \n\t"   // save sp for transferring args later

    "mfps r1                    \n\t"   // save the low byte of the current PSW
    "spl 7                      \n\t"   // Set the processor priority level to 7, disabling all interrupts

    "mov r5,-(sp)               \n\t"   // push r5
    "mov r4,-(sp)               \n\t"   // push r4
    "mov r3,-(sp)               \n\t"   // push r3
    "mov r2,-(sp)               \n\t"   // push r2
    "mov _pending_chain,-(sp)   \n\t"   // push/link the current thread at the top of the pending chain
    "mov sp,_pending_chain      \n\t"   // save this thread as the new top of the pending chain

    "mov (r0)+,r2               \n\t"   // skip over the return address
    "mov (r0)+,r2               \n\t"   // get the address of the code
    "mov (r0)+,sp               \n\t"   // get the new stack pointer
    "clr -(sp)                  \n\t"   // push the termination flag to the new stack
    "mov 2(r0),-(sp)            \n\t"   // push two args to the new stack
    "mov (r0),-(sp)             \n\t"   //

    "mtps r1                    \n\t"   // Restore the low byte of the PSW from previous_state
    "jsr pc,(r2)                \n\t"   // call the new thread

    // When the new thread terminates it returns here with a return value in r0
    // The sp points to where it was when the thread was called

    "mov r0,2(sp)               \n\t"   // save the return value in the second from last word on the stack (possibly overwriting an arg which is no longer needed)
    "mov $1,4(sp)               \n\t"   // set the thread termination flag to true

    // resume the first thread on the pending chain

    "mfps r1                    \n\t"   // save the low byte of the current PSW
    "spl 7                      \n\t"   // Set the processor priority level to 7, disabling all interrupts

    "mov _pending_chain,sp      \n\t"   // get the top of the pending chain
    "mov (sp)+, _pending_chain  \n\t"   // unlink the next thread on the chain
    "mov (sp)+,r2               \n\t"   // restore r2
    "mov (sp)+,r3               \n\t"   // restore r3
    "mov (sp)+,r4               \n\t"   // restore r4
    "mov (sp)+,r5               \n\t"   // restore r5
    "mtps r1                    \n\t"   // Restore the low byte of the PSW from previous_state
    );
    }


