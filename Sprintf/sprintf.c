/*
 * The original source was downloaded from:
 * http://www.menie.org/georges/embedded/index.html
 *
 * Edit history:
 * Jonathan Engdahl xx-SEP-2010 Adapted from the original download.
 * Jonathan Engdahl 04-Jun-2013 Added dummy "l" qualifier.
 * Jonathan Engdahl 06-Sep-2016 Fix conversion of 64-bit numbers.
 * Jonathan Engdahl 13-Feb-2024 Port to PDP-11
 */


 /*
    Copyright
        2001, 2002 Georges Menie (www.menie.org)
    stdarg version contributed by Christian Ettinger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <stdarg.h> /* for varargs stuff */
#include <stdio.h>  /* for std I/O function declarations */
#include <string.h> /* for sprintf function declaration */
#include <stdint.h>


static inline void writechar( char *pCh, unsigned c )
    {
    *pCh = c;
    }

static void printchar(char **str, int c)
    {
    writechar( *str, c );
    ++(*str);
    }

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, char *bufend, const char *string, int width, int pad)
{
    int pc = 0, padchar = ' ';

    if (width > 0) {
        int len = 0;
        const char *ptr;
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for ( ; width > 0 && *out<bufend; --width) {
            printchar (out, padchar);
            ++pc;
        }
    }
    for ( ; *string && *out<bufend; ++string) {
        printchar (out, *string);
        ++pc;
    }
    for ( ; width > 0 && *out<bufend; --width) {
        printchar (out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 64 bit int */
#define PRINT_BUF_LEN 20

/*
    Name:       printi
                Format a number into a print buffer.

    args:
        out     pointer to current position in output buffer
        bufend  pointer to last byte of output buffer
        i       number to convert
        b       base
        sg      sign flag, 0= unsigned, 1 = signed
        width   field width
        pad     pad flags
        letbase 'a' or 'A', depending on whether alpha characters are to be lower or upper case
*/

static int printi(char **out, char *bufend, long i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    char *s;
    int t, neg = 0, pc = 0;
    unsigned long u = i;

    if (i == 0) {
        writechar( &print_buf[0], '0' );
        writechar( &print_buf[1], '\0' );
        return prints (out, bufend, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    writechar( s, '\0' );

    while (u) {
        t = u % b;
        if ( t >= 10 ) {
            t += letbase - '0' - 10;
        }
        writechar( --s, t + '0' );
        u /= b;
    }

    if (neg) {
        if ( width && (pad & PAD_ZERO) && *out<bufend ) {
            printchar (out, '-');
            ++pc;
            --width;
        }
        else {
            writechar( --s, '-' );
        }
    }

    return pc + prints (out, bufend, s, width, pad);
}

int vsnprintf(char *buf, size_t size, const char *format, va_list args )
{
    int width, frac, pad;
    int pc = 0;
    char *out = buf;
    char *bufend = out+size-1;
    char scr[2];
    int longflag = 0;
    long value;

    for (; *format != 0  && out<bufend; ++format) {
        if (*format == '%') {
            ++format;
            width = frac = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for ( ; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if(*format == '.') {
                ++format;
                for ( ; *format >= '0' && *format <= '9'; ++format) {
                    frac *= 10;
                    frac += *format - '0';
                }
            }
            if ( *format == 'l') {
                longflag = 1;
                ++format;
            }
            if ( *format == 'z') {
                if(sizeof(unsigned *) > sizeof(unsigned)) {
                    longflag = 1;
                }
                ++format;
            }
            if ( *format == 's' ) {
                char *s = (char *)va_arg( args, unsigned );
                pc += prints (&out, bufend, s?s:"(null)", width, pad);
                continue;
            }
            if ( *format == 'd' ) {
                if(longflag)value = va_arg( args, long );
                else        value = va_arg( args, int );
                pc += printi (&out, bufend, value, 10, 1, width, pad, 'a');
                continue;
            }
            if ( *format == 'x' ) {
                if(longflag)value = va_arg( args, long );
                else        value = va_arg( args, int ) & 0xFFFF;
                pc += printi (&out, bufend, value, 16, 0, width, pad, 'a');
                continue;
            }
            if ( *format == 'X' ) {
                if(longflag)value = va_arg( args, long );
                else        value = va_arg( args, int ) & 0xFFFF;
                pc += printi (&out, bufend, value, 16, 0, width, pad, 'A');
                continue;
            }
            if ( *format == 'p') {

                if(sizeof(unsigned *) > sizeof(unsigned))longflag=1;
                if(longflag)value = va_arg( args, long );
                else        value = va_arg( args, int ) & 0xFFFF;
                //printchar (&out, '0');
                //printchar (&out, 'x');
                pc += printi (&out, bufend, value, 16, 0, width, pad, 'A');
                continue;
            }
            if ( *format == 'u' ) {
                if(longflag)value = va_arg( args, long );
                else        value = va_arg( args, int ) & 0xFFFF;
                pc += printi (&out, bufend, value, 10, 0, width, pad, 'a');
                continue;
            }
            // An abbreviated implementation of %f. This may not handle negative numbers correctly, I have not thought about it.
            // Ignore the number of fraction digits for now, and always print 3.
            #if 0
            #ifdef __SIZEOF_FLOAT__
            if ( *format == 'f' || *format == 'g' || *format == 'e' ) {
                union
                    {
                    double value;
                    uint64_t i;
                    } v;

                v.value = va_arg( args, double );
                if(v.i == 0x7FF0000000000000)
                    {
                    pc += prints (&out, bufend, "inf", width, pad);
                    continue;
                    }
                pc += printi (&out, bufend, (long)v.value, 10, 1, width-4, pad, 'a');
                pc += prints (&out, bufend, ".", 1, 0);
                pc += printi (&out, bufend, (long)((v.value*1000-(long)v.value*1000)), 10, 0, 3, PAD_ZERO, 'a');
                continue;
            }
            #endif
            #endif
            if ( *format == 'c' ) {
                /* char are converted to int then pushed on the stack */
                writechar( &scr[0], (char)va_arg( args, int ) );
                writechar( &scr[1], '\0' );
                pc += prints (&out, bufend, scr, width, pad);
                continue;
            }
        }
        else {
        out:
            if (out<bufend) {
                printchar (&out, *format);
                ++pc;
            }
        }
    }
    if (out) {
        writechar( out, '\0' );
    }
    va_end( args );
    return pc;
}


int snprintf(char *out, size_t size, const char *format, ...)
    {
    va_list args;

    va_start(args, format);

    return vsnprintf(out, size, format, args);
    }


int vsprintf(char *out, const char *format, va_list args )
    {
    return vsnprintf(out, 255, format, args);
    }

int sprintf(char *out, const char *format, ...)
    {
    va_list args;

    va_start(args, format);
    return vsnprintf(out, 255, format, args);
    }


