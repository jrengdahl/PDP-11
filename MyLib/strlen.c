
unsigned strlen(const char *s)
    {
    unsigned n=0;

    while(*s++ != 0)
        {
        ++n;
        }

    return n;
    }
