extern long int strtol(const char *p, char **endptr, int base);

int atoi(const char *nptr)
    {
    return strtol(nptr, 0, 10);
    }
