#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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

void __main()
    {
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

static struct _reent fake_reent;

struct _reent *impure_ptr = &fake_reent;
