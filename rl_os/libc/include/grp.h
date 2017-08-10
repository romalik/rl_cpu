#ifndef __GRP_H
#define __GRP_H
#include <types.h>
#include <stdio.h>

#define GR_MAX_GROUPS	32
#define GR_MAX_MEMBERS	16

/* The group structure */
struct group {
	char	*gr_name;	/* Group name.	*/
	char	*gr_passwd;	/* Password.	*/
	int	gr_gid; 	/* Group ID.	*/
	char	**gr_mem;	/* Member list. */
};

void setgrent(void);
void endgrent(void);
struct group *getgrent(void);

struct group *getgrgid(const gid_t __gid);
struct group *getgrnam(const char *__name);

struct group *fgetgrent(FILE *__file);

int initgroups(const char *__user, gid_t __gid);

struct group * __getgrent(int __grp_fd);

//char *_path_group;

#endif /* _GRP_H */
