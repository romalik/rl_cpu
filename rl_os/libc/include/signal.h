#ifndef __SIGNAL_H
#define __SIGNAL_H
#ifndef __TYPES_H
#include <types.h>
#endif

#define NSIGS	  32      /* Number of signals <= 32 */
#define SIGHUP		 1
//#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
//#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
//#define SIGPIPE		13
//#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
//#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31


//override!
//
#define SIGALRM 0
#define SIGCHLD 1
#define SIGINT 2
#define SIGKILL 3
#define SIGNUM 4
#define SIGPIPE 5

#define SIG_BLOCK 100
#define SIG_UNBLOCK 101
#define SIG_SETMASK 102

typedef void (*sig_t) (int);

/* signals values */
typedef enum {
	__NOTASIGNAL = 0,
	_SIGLAST = 30000
} signal_t;

#define sigmask(sig) (1UL<<((sig)-1)) 	/* signal mask */

typedef uint32_t sigset_t;	/* at least 16 bits: use 32 in user space */
				/* for expansion space */
/* Type of a signal handler.  */
typedef void (*sighandler_t)(int);

#define SIG_DFL ((sighandler_t)0)	/* default signal handling */
#define SIG_IGN ((sighandler_t)1)	/* ignore signal */
#define SIG_ERR ((sighandler_t)-1)	/* error return from signal */

extern const char *sys_siglist[];

//extern void sighold(int __sig);
//extern void sigrelse(int __sig);
//extern void sigignore(int __sig);
//extern sighandler_t sigset(int __sig, sighandler_t __disp);

int raise(int __sig);

typedef int sig_atomic_t;

sighandler_t signal(int signum, sighandler_t sighandler);

struct  sigaction {
    sig_t   sa_handler;         /* signal handler */
    sigset_t sa_mask;           /* signal mask to apply */
    int     sa_flags;           /* see signal options below */
};

struct  sigvec {
    sig_t   sv_handler;         /* signal handler */
    long    sv_mask;            /* signal mask to apply */
    int     sv_flags;           /* see signal options below */
};


#define sigaddset(set, signo)   (*(set) |= 1L << ((signo) - 1), 0)
#define sigdelset(set, signo)   (*(set) &= ~(1L << ((signo) - 1)), 0)
#define sigemptyset(set)        (*(set) = (sigset_t)0, (int)0)
#define sigfillset(set)         (*(set) = ~(sigset_t)0, (int)0)
#define sigismember(set, signo) ((*(set) & (1L << ((signo) - 1))) != 0)


sig_t   signal (int, sig_t);
int     sigaction (int signum, const struct sigaction *act,
                   struct sigaction *oldact);
int     sigvec (int sig, struct sigvec *vec, struct sigvec *ovec);
int     kill (pid_t pid, int sig);
int     sigpause (int mask);
int     sigblock (int mask);
int     sigsetmask (int mask);
int     sigprocmask (int how, const sigset_t *set, sigset_t *oldset);
int     siginterrupt (int sig, int flag);
int     sigsuspend (const sigset_t *mask);


#endif
