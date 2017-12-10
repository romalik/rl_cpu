#ifndef __SETJMP_H
#define __SETJMP_H
#include <types.h>

        typedef uint16_t jmp_buf[11];
        int setjmp(jmp_buf __env);
        void longjmp (jmp_buf env, int val);
#endif
