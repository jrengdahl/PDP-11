#ifndef LOCAL_H
#define LOCAL_H

#define MAXPRINTF 128
#define INBUFLEN 80

extern int getdec(const char **p);
extern uintptr_t gethex(const char **p);
extern void skip(const char **p);
extern void getline(char *buf, int size);
extern void dump(void *pp, int size);


#endif // LOCAL_H
