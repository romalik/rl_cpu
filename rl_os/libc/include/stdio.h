
#ifndef __STDIO_H
#define __STDIO_H

#include <features.h>
#include <sys/types.h>
#include <stdarg.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define _IOFBF		0x00	/* full buffering */
#define _IOLBF		0x01	/* line buffering */
#define _IONBF		0x02	/* no buffering */
#define __MODE_BUF	0x03	/* Modal buffering dependent on isatty */

#define __MODE_FREEBUF	0x04	/* Buffer allocated with malloc, can free */
#define __MODE_FREEFIL	0x08	/* FILE allocated with malloc, can free */

#define __MODE_READ	0x10	/* Opened in read only */
#define __MODE_WRITE	0x20	/* Opened in write only */
#define __MODE_RDWR	0x30	/* Opened in read/write */

#define __MODE_READING	0x40	/* Buffer has pending read data */
#define __MODE_WRITING	0x80	/* Buffer has pending write data */

#define __MODE_EOF	0x100	/* EOF status */
#define __MODE_ERR	0x200	/* Error status */
#define __MODE_UNGOT	0x400	/* Buffer has been polluted by ungetc */

#ifdef __MSDOS__
#define __MODE_IOTRAN	0x1000	/* MSDOS nl <-> cr,nl translation */
#else
#define __MODE_IOTRAN	0
#endif

/* when you add or change fields here, be sure to change the initialization
 * in stdio_init and fopen */
struct __stdio_file {
  unsigned char *bufpos;   /* the next byte to write to or read from */
  unsigned char *bufread;  /* the end of data returned by last read() */
  unsigned char *bufwrite; /* highest address writable by macro */
  unsigned char *bufstart; /* the start of the buffer */
  unsigned char *bufend;   /* the end of the buffer; ie the byte after the last
                              malloc()ed byte */

  int fd; /* the file descriptor associated with the stream */
  int mode;

  unsigned char unbuf[8];	   /* The buffer for 'unbuffered' streams */

  struct __stdio_file * next;
};

#define EOF	(-1)
#ifndef NULL
#define NULL	((void*)0)
#endif

typedef struct __stdio_file FILE;

#define BUFSIZ	(256)

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int getc(FILE * stream);
int putc(int c, FILE * stream);

#define putchar(c) putc((c), stdout)
#define getchar() getc(stdin)

#define ferror(fp)	(((fp)->mode&__MODE_ERR) != 0)
#define feof(fp)   	(((fp)->mode&__MODE_EOF) != 0)
#define clearerr(fp)	((fp)->mode &= ~(__MODE_EOF|__MODE_ERR),0)
#define fileno(fp)	((fp)->fd)

/* declare functions; not like it makes much difference without ANSI */
/* RDB: The return values _are_ important, especially if we ever use
        8086 'large' model
 */

/* These two call malloc */
#define setlinebuf(__fp)             setvbuf((__fp), (char*)0, _IOLBF, 0)
int setvbuf (FILE*, char*, int, size_t);

/* These don't */
#define setbuf(__fp, __buf) setbuffer((__fp), (__buf), BUFSIZ)
void setbuffer (FILE*, char*, int);

int fgetc (FILE*);
int fputc (int, FILE*);

int fclose (FILE*);
int fflush (FILE*);
char *fgets (char*, size_t, FILE*);

FILE *fopen (char*, char*);
FILE *fdopen (int, char*);
FILE *freopen  (char*, char*, FILE*);

FILE *__fopen (char*, int, FILE*, char*);

int fputs (char*, FILE*);
int puts (char*);

int printf  (const char*, ...);
int fprintf  (FILE*, const char*, ...);
int sprintf  (char*, const char*, ...);

int snprintf(char *__str, size_t __size, const char *__fmt, ...);

int vprintf(const char*, va_list __ap);
int vfprintf(FILE *__stream, const char*, va_list __ap);
int _vfnprintf(FILE *__stream, size_t __size, const char *__fmt, va_list __ap);
int vsprintf(char *__str, const char *__fmt, va_list __ap);
int vsnprintf(char *__str, size_t __size, const char *__fmt, va_list __ap);



int fread(char * buf, int size, int nelm, FILE * fp);
int fwrite(char * buf, int size, int nelm, FILE * fp);

#define stdio_pending(fp) ((fp)->bufread>(fp)->bufpos)

#endif /* __STDIO_H */
