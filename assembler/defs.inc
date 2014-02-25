#define CALL(X) lpc 0 0 *__SP; \
                add **__SP 12 *__SP; \
                sub *__SP 1 __SP; \
                jmp 0 0 X;

#define RET()   add *__SP 1 __SP; \
                jmp 0 0 **__SP;

