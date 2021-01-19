#ifndef _SNP_H
#define _SNP_H

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>


/*COMMON MACROS*/
#define MAXLINE 4096

/*Daemonize*/
void daemonize();

/*error*/
void err_msg(const char *, ...);   /* {App misc_source} */
void err_dump(const char *, ...) __attribute__((noreturn));
void err_quit(const char *, ...) __attribute__((noreturn));
void err_cont(int, const char *, ...);
void err_exit(int, const char *, ...) __attribute__((noreturn));
void err_ret(const char *, ...);
void err_sys(const char *, ...) __attribute__((noreturn));

/* Mac OS pthread_barrier implementation */

#ifdef _DARWIN_C_SOURCE

#ifndef PTHREAD_BARRIER_SERIAL_THREAD
# define PTHREAD_BARRIER_SERIAL_THREAD -1
#endif

typedef pthread_mutexattr_t pthread_barrierattr_t;

/* structure for internal use that should be considered opaque */
typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned count;
	unsigned left;
	unsigned round;
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t *__restrict barrier,
                         const pthread_barrierattr_t * __restrict attr,
                         unsigned count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);

int pthread_barrier_wait(pthread_barrier_t *barrier);

int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict attr,
                                   int *__restrict pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr,
                                   int pshared);

#endif /* _DARWIN_C_SOURCE */

#endif /* _SNP_H */