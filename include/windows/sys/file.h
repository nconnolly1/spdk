#define LOCK_EX 1
#define LOCK_NB 2
extern int flock(int __fd, int __operation);