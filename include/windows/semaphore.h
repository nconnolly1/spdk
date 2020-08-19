typedef struct __sem_s {
	int lock;
} sem_t;

extern int sem_init(sem_t *__sem, int __pshared, unsigned int __value);
extern int sem_destroy(sem_t *__sem);
extern sem_t *sem_open(const char *__name, int __oflag, ...);
extern int sem_close(sem_t *__sem);
extern int sem_unlink (const char *__name);
extern int sem_wait(sem_t *__sem);
extern int sem_post(sem_t *__sem);
