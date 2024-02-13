#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define UNUSED __attribute__((__unused__))

void __assert_func (
    const char      * __file,
    int             __line,
    const char      * __function,
    const char      * __assertion)
    {
    printf("assert(%s) failed in function %s, file %s, at line %d\n",__assertion,__function,__file,__line);
    while(true);
    }

void __cxa_pure_virtual()
    {
    printf("pure virtual called!!!\n");
    assert(false);
    }

void exit( int exit_code UNUSED)
    {
    printf("exit called!!!\n");
    assert(false);
    }

int __errno = 0;

int fflush(FILE * UNUSED)
    {
    return 0;
    }
