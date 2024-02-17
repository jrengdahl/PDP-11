// generic interface to just about any UART

#include <stdint.h>
#include <stdbool.h>
#include "UartParms.h"


bool ControlC = false;
static char nextchar = 0;

#define MEM(x) (*(unsigned short volatile *)(x))


static inline bool uart_kbhit()
    {
    return (MEM(UART_RX_STATUS_REG) & (1 << UART_RX_STATUS_BIT)) == (UART_RX_STATUS_VALUE << UART_RX_STATUS_BIT);
    }

static inline bool uart_txready()
    {
    return (MEM(UART_TX_STATUS_REG) & (1 << UART_TX_STATUS_BIT)) == (UART_TX_STATUS_VALUE << UART_TX_STATUS_BIT);
    }


char uart_inchar()
    {
    char c;

    while (nextchar == 0 && !uart_kbhit())
        {
        }

    if(nextchar != 0)
        {
        c = nextchar;
        nextchar = 0;
        }
    else
        {
        c = (char)(MEM(UART_RX_DATA_REG) & 0xff);
        }

    if(c == '\r')
        {
        c = '\n';
        }

    return c;
    }

void uart_outchar(char c)
    {
    if(c == '\n')
        {
        while (!uart_txready())
            {
            }
   
        MEM(UART_TX_DATA_REG) = '\r';
        }
    
    while (!uart_txready())
        {
        }
   
    MEM(UART_TX_DATA_REG) = c;
    }


bool uart_poll()
    {
    if(uart_kbhit())
        {
        nextchar = (char)(MEM(UART_RX_DATA_REG) & 0xff);
        if(nextchar == 'C'- 0x40)
            {
            ControlC = true;
            nextchar = 0;
            }
        return true;
        }
    else
        {
        return false;
        }
    }


