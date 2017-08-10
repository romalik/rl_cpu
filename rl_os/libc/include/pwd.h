#ifndef __PWD_H
#define __PWD_H
#include <types.h>
#include <stdio.h>

/* The passwd structure.  */
struct passwd {
	char	*pw_name;	/* Username */
	char	*pw_passwd;	/* Password */
	uid_t	pw_uid; 	/* User ID */
	gid_t	pw_gid; 	/* Group ID */
	char	*pw_gecos;	/* Real name */
	char	*pw_dir;	/* Home directory */
	char	*pw_shell;	/* Shell program */
};

void setpwent(void);
void endpwent(void);
struct passwd *getpwent(void);

int putpwent(const struct passwd *__p, FILE *__f);
int getpw(uid_t uid, char *buf);

struct passwd *fgetpwent(FILE *__file);

struct passwd *getpwuid(uid_t __uid);
struct passwd *getpwnam(const char *__name);

struct passwd * __getpwent(int __passwd_fd);

//WTF?
//char *_path_passwd;

#endif /* pwd.h  */
