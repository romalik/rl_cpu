#ifndef __UTIME_H
#define __UTIME_H
#include <sys/types.h>

struct utimbuf {
    time_t actime;  /* access time */
    time_t modtime; /* modification time */
};


#endif
