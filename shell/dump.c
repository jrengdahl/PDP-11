#include <stdio.h>
#include <stdbool.h>

// memory dump

// Copyright (c) 2009, 2024 Jonathan Engdahl
// BSD license -- see the accompanying LICENSE file

#define BUFLEN 80
static char buf[BUFLEN];
extern bool ControlC;

void dump(void *pp, int size)
    {
    int i;
    int c;
    unsigned char *p = (unsigned char *)pp;
    char *b;

    while(size>0 && !ControlC)
        {
        b = buf;
        b += sprintf(b,"%04x: ",(int)p);
        for(i=0;i<16;i+=2)
            {
            if(i<size)sprintf(b,"%04x ",*(int *)&p[i]);
            else sprintf(b,"     ");
            b += 5;
            }
        *b++ = ' ';
        *b++ = '|';
        for(i=0;i<16;i++)
            {
            if(i<size)
                {
                c=p[i];
                if(' '<=c&&c<0x7f)*b++ = c;
                else *b++ = '.';
                }
            }
        *b++ = '|';
        *b++ = 0;
        p += 16;
        size -= 16;
        puts(buf);
        }
    putchar('\n');  
    }
