/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2019 Intel Corporation
 */

#ifndef _UNISTD_H_
#define _UNISTD_H_
/**
 * This file is added to support common code in eal_common_lcore.c
 * as Microsoft libc does not contain unistd.h. This may be removed
 * in future releases.
 */

typedef unsigned long pid_t;
extern pid_t getpid(void);
extern int sleep(unsigned int secs);
extern int usleep(unsigned long seconds); // check type

extern int access(const char *name, int type);
extern int ftruncate(int __fd, off_t __length);	/* 64-bit? */
extern int open(const char *__file, int __oflag, ...);
extern ssize_t read(int __fd, void *__buf, size_t __nbytes);
extern ssize_t write(int __fd, void *__buf, size_t __nbytes);
extern int close(int fd);

extern char *ttyname(int __fd);
extern int isatty(int __fd);

#define R_OK 4
#define STDERR_FILENO 2

#define _SC_NPROCESSORS_CONF 1
extern long int sysconf(int __name);

// HACK - probably belong elsewhere

extern int mkstemp(char *s);

typedef struct
{
  unsigned long int __val[1024 / sizeof(unsigned long)];
} sigset_t;

typedef uint32_t u_int32_t;

// HACK
#define CPU_CLR(cpu, set) (0)

#define F_GETFL 1
#define F_SETFL 2
#define O_NONBLOCK	16
extern int fcntl(int __fd, int __cmd, ...);

extern long int random (void);
extern void srandom (unsigned int __seed);
extern int rand_r(unsigned int *__seed);

#define CLOCK_REALTIME 1
extern int clock_gettime(int id, struct timespec *__tp);

//typedef void (*sighandler_t) (int);
typedef _crt_signal_t sighandler_t;

struct sigaction {
	sigset_t sa_mask;
	sighandler_t sa_handler;
};


#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGKILL 9
#define SIGPIPE 13
#define SIGUSR1 16

extern int sigemptyset(sigset_t *__set);
extern int sigaction(int __sig, const struct sigaction *act, struct sigaction *oact);
extern int sigaddset(sigset_t *__set, int __signo);

#define SIG_UNBLOCK 1
extern int pthread_sigmask (int __how, const sigset_t *newmask, sigset_t *oldmask);

extern int posix_memalign(void **__memptr, size_t __alignment, size_t __size);

#endif /* _UNISTD_H_ */
