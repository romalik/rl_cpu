#ifndef __TIME_H
#define __TIME_H
#ifndef __TYPES_H
#include <types.h>
#endif
#include <stddef.h>
#include <syscall.h>

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

struct timezone {
	int tz_minuteswest;	/* minutes west of Greenwich */
	int tz_dsttime; 	/* type of dst correction */
};

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

#define __isleap(year)	\
	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

extern char *tzname[2];
extern int daylight;
extern long timezone;

clock_t clock(void);
//time_t mktime(struct tm * __tp);
double difftime(time_t *__time2, time_t *__time1);


void __tm_conv(struct tm *tmbuf, time_t *t, int offset);
char *asctime(struct tm * __tp);
char *asctime_r(struct tm *, char * __buf);
char *ctime(time_t * __tp);
char *ctime_r(time_t * __tp, char * __buf);
void tzset(void);

struct tm *gmtime(time_t *__tp);
struct tm *localtime(time_t * __tp);
struct tm *gmtime_r(time_t *__tvec, struct tm *__result);
struct tm *localtime_r(time_t *__tvec, struct tm *__result);

typedef int clockid_t;

#define CLOCKS_PER_SEC	100		/* FIXME: sysconf */

#define CLOCK_REALTIME	0
#define CLOCK_MONOTONIC 1

int clock_getres(clockid_t __clk_id,  struct timespec *__res);
int clock_gettime(clockid_t __clk_id,  struct timespec *__tp);
int clock_nanosleep(clockid_t __clk_id, int __flags,
	const struct timespec *__request, struct timespec *__remain);
int clock_settime(clockid_t __clk_id,  const struct timespec *__tp);

#define TIMER_ABSTIME	1

int nanosleep(const struct timespec *__request, struct timespec *__remain);

#endif
