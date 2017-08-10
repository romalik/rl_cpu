/* ctype.h	Character classification and conversion
 */
#ifndef __CTYPE_H
#define __CTYPE_H

#include <stdint.h>

int toupper(int __c);
int tolower(int __c);

int isalnum(int __c);
int isalpha(int __c);
int isascii(int __c);
int isblank(int __c);
int iscntrl(int __c);
int isdigit(int __c);
int isgraph(int __c);
int islower(int __c);
int isprint(int __c);
int ispunct(int __c);
int isspace(int __c);
int isupper(int __c);
int isxdigit(int __c);

#define isascii(c) (!((uint8_t)(c) & 0x80))
#define toascii(c) ((c) & 0x7f)

#endif /* __CTYPE_H */
