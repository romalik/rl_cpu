#include "string.h"
#include "kstdio.h"
#include <memmap.h>
#include <syscall.h>

void itoa(int n, char s[]);

unsigned int gettime() {
    struct gettimeSyscall {
        unsigned int id;
        unsigned int c;
    } s;
    s.id = __NR_time;
    s.c = 0;
    syscall(&s);
    return s.c;
}

int getc() {
    int c;
    while ((c = UART) == 0) {
    }
    return c;
}

int putc(char c) {
    PORT_OUT = c;
    return 0;
}

int puts(char *s) {
    while (*s) {
        putc(*s);
        s++;
    }
    return 0;
}

void putDigit(int a) {
    if (a < 0) {
        a = -a;
        putc('-');
    }
    putc('0' + a);
}

void putNum(int a) {
    char buffer[20];
    itoa(a, buffer);
    puts(buffer);
}

static void printchar(char **str, int c) {
    if (str) {
        **str = c;
        ++(*str);
    } else
        (void)putc(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad) {
    int pc = 0, padchar = ' ';

    if (width > 0) {
        int len = 0;
        const char *ptr;
        for (ptr = string; *ptr; ++ptr)
            ++len;
        if (len >= width)
            width = 0;
        else
            width -= len;
        if (pad & PAD_ZERO)
            padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for (; width > 0; --width) {
            printchar(out, padchar);
            ++pc;
        }
    }
    for (; *string; ++string) {
        printchar(out, *string);
        ++pc;
    }
    for (; width > 0; --width) {
        printchar(out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad,
                  int letbase) {
    char print_buf[PRINT_BUF_LEN];
    char *s;
    int t, neg = 0, pc = 0;
    unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    if (b == 16) {
        while (u) {
            t = u & 0xf;
            if (t >= 10)
                t += letbase - '0' - 10;
            *--s = t + '0';
            u = u >> 4;
        }
    } else {
        while (u) {
            t = u % b;
            if (t >= 10)
                t += letbase - '0' - 10;
            *--s = t + '0';
            u /= b;
        }
    }

    if (neg) {
        if (width && (pad & PAD_ZERO)) {
            printchar(out, '-');
            ++pc;
            --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}

static int print(char **out, int *varg) {
    int width, pad;
    int pc = 0;
    char *format = (char *)(*varg++);
    char scr[2];
    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0')
                break;
            if (*format == '%')
                goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for (; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if (*format == 's') {
                char *s = *((char **)varg++);
                pc += prints(out, s ? s : "(null)", width, pad);
                continue;
            }
            if (*format == 'd') {
                pc += printi(out, *varg++, 10, 1, width, pad, 'a');
                continue;
            }
            if (*format == 'x') {
                pc += printi(out, *varg++, 16, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'X') {
                pc += printi(out, *varg++, 16, 0, width, pad, 'A');
                continue;
            }
            if (*format == 'u') {
                pc += printi(out, *varg++, 10, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'c') {
                /* char are converted to int then pushed on the stack */
                scr[0] = *varg++;
                scr[1] = '\0';
                pc += prints(out, scr, width, pad);
                continue;
            }
        } else {
        out:
            printchar(out, *format);
            ++pc;
        }
    }
    if (out)
        **out = '\0';
    return pc;
}

/* assuming sizeof(void *) == sizeof(int) */

int printf(const char *format, ...) {
    int *varg = (int *)(&format);
    return print(0, varg);
}

int sprintf(char *out, const char *format, ...) {
    int *varg = (int *)(&format);
    return print(&out, varg);
}
