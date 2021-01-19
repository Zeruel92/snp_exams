#ifndef _SNP_H
#define _SNP_H
#define _POSIX_C_SOURCE 200809L

#if defined(SOLARIS)		/* Solaris 10 */
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif

#include <sys/types.h>		/* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>	/* for winsize */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif

#include <stdio.h>		/* for convenience */
#include <stdlib.h>		/* for convenience */
#include <stddef.h>		/* for offsetof */
#include <string.h>		/* for convenience */
#include <unistd.h>		/* for convenience */
#include <signal.h>		/* for SIG_ERR */

#define	MAXLINE	4096			/* max line length */

/*
 * Default file access permissions for new files.
 */
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Default permissions for new directories.
 */
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/*
 * Demonize function
 */
void daemonize(const char *cmd);

/*
 * Error functions
 */
void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_cont(int error, const char *fmt, ...);
void err_exit(int error, const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

/*
 * File flags utilities
 */

void clr_fl(int fd, int flags);
int set_cloexec(int fd);
void set_fl(int fd, int flags);

/*
 * path alloc
 */

char * path_alloc(size_t *sizep);

/*
 * Colorized printf
 */

#define RED 1
#define ORANGE 2
#define GREEN 3

void cPrintf(char *str,int color);

/*
 * TTY Modes
 */
int		 tty_cbreak(int);					/* {Prog raw} */
int		 tty_raw(int);						/* {Prog raw} */
int		 tty_reset(int);					/* {Prog raw} */
void	 tty_atexit(void);					/* {Prog raw} */
struct termios	*tty_termios(void);			/* {Prog raw} */
#endif /* _SNP_H END*/
