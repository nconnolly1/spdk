#define MAP_FAILED ((void *)-1)

#define PROT_READ 1
#define PROT_WRITE 2

#define MAP_SHARED 1

extern int shm_open(const char *__name, int __oflag, int __mode);
extern int shm_unlink(const char *__name);

extern void *mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset);	// offset size
extern int munmap(void *__addr, size_t __len);