#ifndef PDP_11_H
#define PDP_11_H

#include <stdint.h>

static inline uint16_t __disable_interrupts(void)
    {
    uint16_t previous_state;

    __asm__ volatile (
        "mfps %0\n\t"               // Move the low byte of the current PSW into previous_state
        "spl 7"                     // Set the processor priority level to 7, disabling all interrupts
        : "=r"(previous_state)      // Output operand: previous_state variable
        :                           // No input operands
        : "memory"                  // Clobbered: memory (to ensure memory is not assumed unchanged by the optimizer)
    );

    return previous_state;
    }


static inline void __restore_interrupts(uint16_t previous_state)
    {
    __asm__ volatile (
        "mtps %0\n\t"               // Restore the low byte of the PSW from previous_state
        :
        : "r"(previous_state)       // Input operand
        : "cc", "memory"            // Clobber condition codes and memory
        );
    }


#endif // PDP_11_h
