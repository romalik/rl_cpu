#ifndef SIGNAL_H_
#define SIGNAL_H_

typedef void (*sighandler_t)(int);

#define SIGALRM 0
#define SIGCHLD 1
#define SIGINT 2
#define SIGKILL 3
#define SIGNUM 4
#define SIGPIPE 13

#define SIG_DFL 0
#define SIG_IGN 1


#endif
