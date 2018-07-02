#include <signal.h>

sighandler_t   signal (int s, sighandler_t handler) {
	return 0;
}
int     sigaction (int signum, const struct sigaction *act, struct sigaction *oldact){ return 0;}
int     sigvec (int sig, struct sigvec *vec, struct sigvec *ovec){return 0;}
int     sigpause (int mask) {return 0;}
int     sigblock (int mask) {return 0;}
int     sigsetmask (int mask) {return 0;}
int     sigprocmask (int how, const sigset_t *set, sigset_t *oldset) {return 0;}
int     siginterrupt (int sig, int flag) {return 0;}
int     sigsuspend (const sigset_t *mask) {return 0;}

