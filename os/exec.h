#ifndef EXEC_H__
#define EXEC_H__

unsigned int execve(unsigned int *filename, unsigned int *argv[],
                    unsigned int *envp[]);

#endif
