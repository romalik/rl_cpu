/* These functions find the absolute path to the current working directory.
 *
 * They don't use malloc or large amounts of stack space.
 */  
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

static char *search_dir(uint, uint);    /* Routine to find the step back down */
static char *recurser(void);    /* Routine to go up tree */
static char *path_buf;
static int path_size;

static uint root_dev;
static uint root_ino;
static struct stat st;

static char *search_dir(uint this_dev, uint this_ino) 
{
        struct dirent *d;
        char *ptr;
        int slen;
        DIR * dp;
        unsigned char slow_search = 0;

        if (stat(path_buf, &st) < 0)
                return NULL;
        if (this_dev != st.st_dev)
                ++slow_search;
        slen = strlen(path_buf);
        ptr = path_buf + slen - 1;
        if (*ptr != '/') {
                if (slen + 2 > path_size) {
                  //printf("E10\n");
                        errno = ERANGE;
                        return NULL;
                }
                strcpy(++ptr, "/");
                ++slen;
        }
        ++slen;
        if ((dp = opendir(path_buf)) == 0) {
          //printf("E11\n");
          return NULL;
        }
        while ((d = readdir(dp)) != 0) {
                if (slow_search || this_ino == d->d_ino) {
                        if (slen + strlen(d->d_name) > path_size) {
                                //printf("E12\n");
                                errno = ERANGE;
                                return NULL;
                        }
                        strcpy(ptr + 1, d->d_name);
                        if (stat(path_buf, &st) < 0)
                                continue;
                        if (st.st_ino == this_ino && st.st_dev == this_dev) {
                                closedir(dp);
                                //printf("Searchdir : %s\n", path_buf);
                                return path_buf;
                        }
                }
                    /* else ??? */ 
        }
        closedir(dp);
        errno = ENOENT;
        //printf("E13\n");
        return NULL;
}

static char *recurser(void) 
{
        uint this_dev;
        uint this_ino;

        if (stat(path_buf, &st) < 0) {
          //printf("E1!\n");
          return NULL;

        }
        this_dev = st.st_dev;
        this_ino = st.st_ino;

        //printf("getcwd trace : this_dev = %d root_dev = %d this_ino = %d root_ino = %d\n",this_dev, root_dev, this_ino, root_ino);
        //printf("getcwd trace : path_buf = %s\n", path_buf);


        if (this_dev == root_dev && this_ino == root_ino) {
          //printf("Hit root\n");
                strcpy(path_buf, "/");
                return path_buf;
        }
        if (strlen(path_buf) + 4 > path_size) {
          //printf("E2!\n");
                errno = ERANGE;
                return NULL;
        }
        strcat(path_buf, "/..");
        return recurser()? search_dir(this_dev, this_ino) : NULL;
}

char *getcwd(char *buf, int size) 
{
        if ((path_size = size) < 3) {
                errno = ERANGE;
                return 0;
        }
        strcpy(path_buf = buf, ".");
        if (stat("/", &st) < 0)
                return NULL;    /* no root */
        root_dev = st.st_dev;
        root_ino = st.st_ino;
        return recurser();
}

