typedef unsigned long int nfds_t;
struct pollfd;

extern int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout);