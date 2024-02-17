// printf, etc. are subset implementations of standard C library routines.
// Use the regular #include <stdio.h>.

// Copyright (c) 2009, 2023 Jonathan Engdahl
// BSD license -- see the accompanying LICENSE file


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "local.h"
#include "uart.h"


char printbuf[MAXPRINTF];                                       // a single printf buffer shared by all threads


// C library printf
int printf(const char *fmt, ...)
    {
    va_list args;
    va_start(args, fmt);

    return vprintf(fmt, args);
    }


// C library vprintf
int vprintf(const char *fmt, va_list args)
    {
    int len = vsnprintf(printbuf, MAXPRINTF, fmt, args);        // format the message into the shared buffer

    for(int i=0; i<len; i++)
        {
        uart_outchar(printbuf[i]);
        }

    return len;
    }


// C library puts
int puts(const char *str)
    {
    unsigned len = strlen(str);

    for(int i=0; i<len; i++)
        {
        uart_outchar(str[i]);
        }

    putchar('\n');

    return len;
    }


// C library putchar
int putchar(const int c)
    {
    uart_outchar(c);

    return c;
    }
