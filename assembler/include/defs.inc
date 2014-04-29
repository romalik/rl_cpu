#define __SP 0x1010
#define REG_A 0x1000
#define REG_B 0x1001
#define REG_C 0x1002
#define REG_D 0x1003
#define REG_E 0x1004
#define REG_F 0x1005
#define REG_LOCAL 0x1006
#define REG_DISPLAY 0xfffe
#define REG_DISPLAY1 0xfffd
#define REG_TTY 0x2000
#define REG_KB 0x2001
#define REG_PORT 0x2002

#define CALL(X) lpc 0 0 *__SP; \
                add **__SP 20 *__SP; \
                sub *__SP 1 __SP; \
                add 0 1 *__SP; \
                sub *__SP 1 __SP; \
                jmp 0 0 X;

#define CALL1(X,A) lpc 0 0 *__SP; \
                add **__SP 28 *__SP; \
                sub *__SP 1 __SP; \
                add 0 A *__SP; \
                sub *__SP 1 __SP; \
                add 0 2 *__SP; \
                sub *__SP 1 __SP; \
                jmp 0 0 X;

#define CALL2(X,A,B) lpc 0 0 *__SP; \
                add **__SP 36 *__SP; \
                sub *__SP 1 __SP; \
                add 0 B *__SP; \
                sub *__SP 1 __SP; \
                add 0 A *__SP; \
                sub *__SP 1 __SP; \
                add 0 3 *__SP; \
                sub *__SP 1 __SP; \
                jmp 0 0 X;
/*
#define CALL(X, A, B) lpc 0 0 *__SP; \
                add **__SP 12 *__SP; \
                sub *__SP 1 __SP; \
                jmp 0 0 X;
*/
#define RET()   add *__SP 1 __SP; \
                add *__SP **__SP __SP; \
                jmp 0 0 **__SP;

#define LOCAL1(A)  add *__SP 2 REG_LOCAL; \
                   add **REG_LOCAL 0 A;

#define TO_LOCAL1(A)  add *__SP 2 REG_LOCAL; \
                      add A 0 *REG_LOCAL;

#define LOCAL2(A)  add *__SP 3 REG_LOCAL; \
                  add **REG_LOCAL 0 A;

#define TO_LOCAL2(A)  add *__SP 3 REG_LOCAL; \
                      add A 0 *REG_LOCAL;

