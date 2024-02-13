extern char *strchr(const char *s, int c);

unsigned strcspn(const char *s, const char *reject)
    {
    unsigned n = 0;

    while (!strchr(reject, *s))
        {
        n++;
        s++;
        }

    return n;
    }
