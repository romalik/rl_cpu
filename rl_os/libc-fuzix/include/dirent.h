#ifndef DIRENT_H__
#define DIRENT_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>


/* `MAXNAMLEN' is the BSD name for what POSIX calls `NAME_MAX'.  */
#define MAXNAMLEN       30
#define NAME_MAX        30

struct dirent {
        ino_t            d_ino;          /* Try to be iBCS2 like */
        off_t           d_off;
        unsigned short  d_reclen;
        char            d_name[31];
};

/* Internal directory structure */
struct _dir {
        struct dirent de;
        uint8_t buf[64];
        uint8_t next;
        uint8_t last;
};

/* Directory stream type.  */
typedef struct {
        int dd_fd;              /* file descriptor */
        int dd_loc;             /* offset in buffer */
        int dd_size;            /* # of valid entries in buffer */
        struct dirent *dd_buf;  /* -> directory buffer */
        struct _dir _priv;
} DIR;                          /* stream data from opendir() */


typedef int (*__dir_select_fn_t)(struct dirent *__de);

typedef int (*__dir_compar_fn_t)(struct dirent **__d1, struct dirent **__d2);

/* Kernel directory format off disk */
struct __dirent {
        char            d_name[31];
        ino_t           d_ino;
};
extern DIR *opendir(char *__name);
extern DIR *opendir_r(DIR *__dirp, char *__name);
extern int closedir(DIR *__dirp);
extern int closedir_r(DIR *__dirp);
extern struct dirent *readdir(DIR *__dirp);
extern void rewinddir(DIR *__dirp);

extern void seekdir(DIR *__dirp, off_t __pos);
extern off_t telldir(DIR *__dirp);

/* Scan the directory DIR, calling SELECT on each directory entry.
 *    Entries for which SELECT returns nonzero are individually malloc'd,
 *       sorted using qsort with CMP, and collected in a malloc'd array in
 *          *NAMELIST.  Returns the number of entries selected, or -1 on error.
 *           */

/*
extern int scandir(char *__dir,
                   struct dirent ***__namelist,
                   __dir_select_fn_t __select,
                   __dir_compar_fn_t __compar);

*/

#endif
