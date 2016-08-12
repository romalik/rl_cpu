#ifndef __STDARG
#define __STDARG

typedef char* va_list;

#define __va_argsiz(t) sizeof(t)

#define va_start(ap, pN) ((ap) = ((va_list) (&pN) + __va_argsiz(pN)))

#define va_end(ap) ((void)0)

#define va_arg(ap, t) (((ap) = (ap) + __va_argsiz(t)) , *((t *) (void *) ((ap) - __va_argsiz(t))))

#endif
