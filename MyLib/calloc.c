// this is an extremely lightweight calloc
// I assume that nothing is ever freed

extern void *malloc(unsigned s);
extern void *memset(void *dest, int c, unsigned n);

void *calloc(unsigned num, unsigned size)
    {
    char *tmp;

    tmp = (char *)malloc(num*size);     // get the block of memory
    memset(tmp,0,num*size);             // set the memory to zero
    return (void *)tmp;
    }
