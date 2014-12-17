#ifndef __DEFS_INC
#define __DEFS_INC

#define INC(x) INC_ ## x
#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INC_9 10


#define LOCAL(X) *BP-X

#define ARG(X) *BP+INC(X)

#define FUNC_HEADER()    push *BP 0 0 __CR__ \
						add SP 1 BP __CR__
 

#define FUNC_FOOTER()   sub *BP 1 SP __CR__\
						pop BP 0 0 __CR__\
						ret 0 0 0 __CR__

#define shl(A,D)    shift A 0 D
#define shr(A,D)    shift A 1 D
#define shl8(A,D)    shift A 2 D
#define shr8(A,D)    shift A 3 D


#endif 
