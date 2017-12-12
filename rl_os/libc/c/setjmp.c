#include <setjmp.h>

        int setjmp(jmp_buf __env) { return 0; }
        void longjmp (jmp_buf env, int val) { return ; }



