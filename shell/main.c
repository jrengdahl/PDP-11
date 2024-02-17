#include <stdio.h>
#include <stdbool.h>
#include "uart.h"

extern void bear();
extern void getline();

char buffer[80];

int main()
    {
    bear();

    while(true)
        {
        putchar('>');
        getline(buffer, 80);
        printf("%s\n", buffer);
        }

    return 0;
    }
