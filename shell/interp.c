// a command line interpreter for debugging the target

// Copyright (c) 2009, 2024 Jonathan Engdahl
// BSD license -- see the accompanying LICENSE file


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "local.h"
#include "uart.h"

extern void bear();
extern char *strchrnul(const char *s, int c);   // POSIX function but not included in newlib, see https://linux.die.net/man/3/strchr


// print a large number with commas
void commas(long x)
    {
    if(x>=1000000000)
        {
        printf("%lu,%03lu,%03lu,%03lu", x/1000000000, x/1000000%1000, x/1000%1000, x%1000);
        } // those are "L"s after the threes, not ones
    else if(x>=1000000)
        {
        printf("%lu,%03lu,%03lu", x/1000000, x/1000%1000, x%1000);
        }
    else if(x>=1000)
        {
        printf("%lu,%03lu", x/1000%1000, x%1000);
        }
    else printf("%lu", x);
    }

// print help string, see usage below
#define HELP(s) else if(buf[0]=='?' && puts(s) && 0){}

char buf[INBUFLEN];

void interp()
    {
    bear();
    printf("hello, world!\n");
    printf("build: %s %s\n", __DATE__, __TIME__);

    while(1)
        {
        const char *p;

        ControlC = false;
        putchar('>');                                                           // output the command line prompt
        getline(buf, INBUFLEN);                                                 // get a command line
        p = buf;
        skip(&p);                                                               // skip command and following whitespace

        if(buf[0]==0 || buf[0]=='\r' || buf[0]=='\n' || buf[0]==' '){}          // ignore blank lines

        HELP(  "Addresses, data, and sizes are generally in hex.")

//              //                              //
        HELP(  "d <addr> {<size>}               dump memory")
        else if(buf[0]=='d' && buf[1]==' ')                                     // memory dump d <hex address> <size>
            {
            unsigned *a;
            int s;

            a = (unsigned *)gethex(&p);                                         // get the first arg
            skip(&p);
            if(isxdigit((int)*p))s = gethex(&p);                                // the second arg is the size
            else s = 4;

            dump(a, s);                                                         // do the dump
            }

//              //                              //
        HELP(  "m <addr> <data>...              modify memory")
        else if(buf[0]=='m' && buf[1]==' ')                                     // memory modify <address> <data> ...
            {
            unsigned *a;
            unsigned v;

            a = (unsigned *)gethex(&p);                                         // get the first arg
            skip(&p);
            while(isxdigit((int)*p))                                            // while there is any data left on the command line
                {
                v = gethex(&p);                                                 // get the data
                skip(&p);                                                       // skip that arg and folowing whitespace
                *a++ = v;                                                       // store the data
                }
            }


//              //                              //
        HELP(  "f <addr> <size> <data>          fill memory")
        else if(buf[0]=='f' && buf[1]==' ')
            {
            unsigned *a;
            unsigned s;
            unsigned v;

            a = (unsigned *)gethex(&p);                     // get the first arg
            skip(&p);
            s = gethex(&p);                                 // the second arg is the size
            skip(&p);
            v = gethex(&p);                                 // value is the third arg

            while(s>0)                                      // fill memory with the value
                {
                *a++ = v;
                s -= sizeof(*a);
                }
            }


#if 0
//              //                              //
        HELP(  "rt <addr> <size>...             ram test")
        else if(buf[0]=='r' && buf[1]=='t')
            {
            uint8_t *addr;
            unsigned size;
            int repeat = 1;
            unsigned limit = 10;

            addr = (uint8_t *)gethex(&p);                   // get the first arg (address)
            skip(&p);
            size = gethex(&p);                              // get second arg (size of area to test)
            skip(&p);
            if(isdigit(*p))                                 // get optional third arg (repeat count)
                {
                repeat = getdec(&p);
                skip(&p);
                if(isdigit(*p))                             // get optional fourth arg (max errors to report)
                    {
                    limit = getdec(&p);
                    }
                }

            RamTest(addr, size, repeat, limit);
            }
#endif

#if 0

//              //                              //
        HELP(  "c <count>                       calibrate")
        else if(buf[0]=='c' && buf[1]==' ')
            {
            int size = 32;
            uint32_t ticks = 0;
            uint32_t lastTIM2 = 0;
            uint32_t elapsedTIM2 = 0;
            float last_wtime;
            float elapsed_wtime;

            uint64_t count = 1;

            if(isdigit(*p))                                                 // while there is any data left on the command line
                {
                count = getlong(&p);                                        // get the count
                skip(&p);
                if(isdigit(*p))                                             // while there is any data left on the command line
                    {
                    size = getdec(&p);                                      // get the size
                    }
                }

            if(size==64)
                {
                __disable_irq();
                last_wtime = omp_get_wtime(0);
                lastTIM2 = __HAL_TIM_GET_COUNTER(&htim2);
                Elapsed();
                bogodelay(count);
                ticks = Elapsed();
                elapsedTIM2 = __HAL_TIM_GET_COUNTER(&htim2) - lastTIM2;
                elapsed_wtime = omp_get_wtime(0) - last_wtime;
                __enable_irq();
                }
            else
                {
                __disable_irq();
                last_wtime = omp_get_wtime(0);
                lastTIM2 = __HAL_TIM_GET_COUNTER(&htim2);
                Elapsed();
                bogodelay((uint32_t)count);
                ticks = Elapsed();
                elapsedTIM2 = __HAL_TIM_GET_COUNTER(&htim2) - lastTIM2;
                elapsed_wtime = omp_get_wtime(0) - last_wtime;
                __enable_irq();
                }
            commas(ticks);
            printf(" microseconds by CYCCNT\n");
            commas(elapsedTIM2);
            printf(" microseconds by TIM2\n");
            printf("%lf microseconds by wtime\n", elapsed_wtime*1000000.0);
            }

#endif


        // print the help screen
        else if(buf[0]=='?')
            {
            }

        // else I dunno what you want
        else printf("illegal command\n");

        }
    }


