#include "apue.h"
#include <dirent.h>
#include <limits.h>

/* function type that is called for each filename */
typedef	int	Myfunc(const char *, const struct stat *, int);

static Myfunc	myfunc;
static int		myftw(char *, Myfunc *);
static int		dopath(Myfunc *);

ino_t inode_stdin;
dev_t device_type_stdin;

int
main(int argc, char *argv[])
{
	int ret;
    struct stat	statbuf;
    
    if(fstat(STDIN_FILENO,&statbuf) < 0)
        err_sys("Impossible to stat STDIN_FILENO\n");
    
    inode_stdin = statbuf.st_ino;
    device_type_stdin = statbuf.st_dev;
    
    printf("Lo standard input sta usando il file con inode %lu inizio la ricerca\n",inode_stdin);
    
	ret = myftw("/", myfunc);		/* does it all */

	
	exit(ret);
}

/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define	FTW_F	1		/* file other than directory */
#define	FTW_D	2		/* directory */
#define	FTW_DNR	3		/* directory that can't be read */
#define	FTW_NS	4		/* file that we can't stat */

static char	*fullpath;		/* contains full pathname for every file */
static size_t pathlen;

static int					/* we return whatever func() returns */
myftw(char *pathname, Myfunc *func)
{
	fullpath = path_alloc(&pathlen);	/* malloc PATH_MAX+1 bytes */
										/* ({Prog pathalloc}) */
	if (pathlen <= strlen(pathname)) {
		pathlen = strlen(pathname) * 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	strcpy(fullpath, pathname);
	return(dopath(func));
}

/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return.  For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int					/* we return whatever func() returns */
dopath(Myfunc* func)
{
	struct stat		statbuf;
	struct dirent	*dirp;
	DIR				*dp;
	int				ret, n;

	if (lstat(fullpath, &statbuf) < 0)	/* stat error */
		return(func(fullpath, &statbuf, FTW_NS));
	if (S_ISDIR(statbuf.st_mode) == 0)	/* not a directory */
		return(func(fullpath, &statbuf, FTW_F));

	/*
	 * It's a directory.  First call func() for the directory,
	 * then process each filename in the directory.
	 */
	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return(ret);

	n = strlen(fullpath);
	if (n + NAME_MAX + 2 > pathlen) {	/* expand path buffer */
		pathlen *= 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	fullpath[n++] = '/';
	fullpath[n] = 0;

	if ((dp = opendir(fullpath)) == NULL)	/* can't read directory */
		return(func(fullpath, &statbuf, FTW_DNR));

	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0  ||
		    strcmp(dirp->d_name, "..") == 0)
				continue;		/* ignore dot and dot-dot */
		strcpy(&fullpath[n], dirp->d_name);	/* append name after "/" */
		if ((ret = dopath(func)) != 0)		/* recursive */
			break;	/* time to leave */
	}
	fullpath[n-1] = 0;	/* erase everything from slash onward */

	if (closedir(dp) < 0)
		err_ret("can't close directory %s", fullpath);
	return(ret);
}

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch (type) {
	case FTW_F:
		if(statptr->st_ino == inode_stdin){
            if(statptr->st_dev == device_type_stdin){
                printf("Stai usando il file %s con inode %lu come standard input per questo processo\n",pathname,inode_stdin);
                exit(0);
            }
        }
		break;
	case FTW_D:
		break;
	case FTW_DNR:
		break;
	case FTW_NS:
		break;
	default:
		err_dump("unknown type %d for pathname %s", type, pathname);
	}
	return(0);
}
