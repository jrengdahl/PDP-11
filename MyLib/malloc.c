#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "PDP-11.h"

extern unsigned _heap_start;
extern unsigned _heap_end;
extern uintptr_t _break;

int malloc_verbose = 0;

typedef struct MemBlock
    {
    union
        {
        struct MemBlock *next;
        unsigned bucket;
        };
    uintptr_t useable;
    }MemBlock;

#define MINSIZE 8
#define MAXSIZE 1024
#define SIZES 8

static MemBlock *FreeBlocks[SIZES];


void *malloc(size_t size)
    {
    MemBlock *blk = 0;
    unsigned bucket;
    uint16_t InterruptState;

    size = (size+7) & -8;                       // round size up to next 8 bytes
    assert(size >= MINSIZE && size <= MAXSIZE); // make sure it's a valid block size
    bucket = 31-__builtin_clz(size>>2);         // get the bucket number

    InterruptState = __disable_interrupts();

    blk = FreeBlocks[bucket];
    if(blk != 0)
        {
        FreeBlocks[bucket] = blk->next;
//        if(malloc_verbose >= 1)printf("malloc old %8p %u %u\n", blk, size, 1<<(bucket+3));
        }
    else
        {
        blk = (MemBlock *)_break;
        _break += (1<<(bucket+3))+4;
        assert(_break < (uintptr_t)&_heap_end);                 // assert that the heap has not overflowed
//        if(malloc_verbose >= 1)printf("malloc new %8p %u %u\n", blk, size, 1<<(bucket+3));
        }

    __restore_interrupts(InterruptState);

    blk->bucket = bucket;

    return (void *)&(blk->useable);
    }


void free(void *ptr)
    {
    MemBlock *blk = (MemBlock *)((uintptr_t)ptr - 4);
    unsigned bucket = blk->bucket;
    uint16_t InterruptState;

//    if(malloc_verbose >= 1)printf("free %8p %u\n", blk, 1<<(bucket+3));

    InterruptState = __disable_interrupts();

    blk->next = FreeBlocks[bucket];
    FreeBlocks[bucket] = blk;

    __restore_interrupts(InterruptState);
    }


void mem()
    {
    printf("%u bytes of heap left\n", (uintptr_t)&_heap_end - _break);

    int size = MINSIZE;
    for(unsigned i=0; i<SIZES; i++)
        {
        printf("%4d byte blocks: ", size);
        for(MemBlock *blk = FreeBlocks[i]; blk != 0; blk = blk->next)
            {
            printf("%p ", blk);
            } 
        printf("\n");
        size *= 2;
        }
    }
