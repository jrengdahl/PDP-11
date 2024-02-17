#ifndef UART_H
#define UART_H

#include <stdbool.h>

extern bool ControlC;

extern char uart_inchar();
extern void uart_outchar(char c);
extern bool uart_poll();

#endif // UART_H

