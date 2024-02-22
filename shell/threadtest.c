#include <stdio.h>
#include "threads11.h"

int teststack[128];

PORT port;

int testthread(int arg1, int arg2)
    {
    int value;

    printf("thread: arg1 = %d, arg2 = %d\n", arg1, arg2);
    value = wait(&port);
    printf("thread: value = %d\n", value);

    return 4;
    }



void threadtest(char **p __attribute__((__unused__)))
    {
    
    spawn(&testthread, THREADSTACK(teststack), 1, 2);
    wake(&port, 3);
    printf("rtest  = %d\n", rendezvous_test(teststack));
    printf("rvalue = %d\n", rendezvous_return_value(teststack));
    }
