pdp11-aout-gcc -m45 -Os -I ../include -c atoi.c
pdp11-aout-gcc -m45 -Os -I ../include -c calloc.c
pdp11-aout-gcc -m45 -Os -I ../include -c malloc.c
pdp11-aout-gcc -m45 -Os -I ../include -c memcmp.c
pdp11-aout-gcc -m45 -Os -I ../include -c memcpy.c
pdp11-aout-gcc -m45 -Os -I ../include -c memmove.c
pdp11-aout-gcc -m45 -Os -I ../include -c memset.c
pdp11-aout-gcc -m45 -Os -I ../include -c sbrk.c
pdp11-aout-gcc -m45 -Os -I ../include -c strcasecmp.c
pdp11-aout-gcc -m45 -Os -I ../include -c strcat.c
pdp11-aout-gcc -m45 -Os -I ../include -c strchr.c
pdp11-aout-gcc -m45 -Os -I ../include -c strchrnul.c
pdp11-aout-gcc -m45 -Os -I ../include -c strcmp.c
pdp11-aout-gcc -m45 -Os -I ../include -c strcpy.c
pdp11-aout-gcc -m45 -Os -I ../include -c strcspn.c
pdp11-aout-gcc -m45 -Os -I ../include -c strlen.c
pdp11-aout-gcc -m45 -Os -I ../include -c strncasecmp.c
pdp11-aout-gcc -m45 -Os -I ../include -c strncat.c
pdp11-aout-gcc -m45 -Os -I ../include -c strnchr.c
pdp11-aout-gcc -m45 -Os -I ../include -c strncmp.c
pdp11-aout-gcc -m45 -Os -I ../include -c strncpy.c
pdp11-aout-gcc -m45 -Os -I ../include -c strnlen.c
pdp11-aout-gcc -m45 -Os -I ../include -c strrchr.c
pdp11-aout-gcc -m45 -Os -I ../include -c strspn.c
pdp11-aout-gcc -m45 -Os -I ../include -c strstr.c
pdp11-aout-gcc -m45 -Os -I ../include -c strtol.c
pdp11-aout-gcc -m45 -Os -I ../include -c stubs.c

pdp11-aout-ar r libMyLib.a *.o
rm -f *.o
