typedef unsigned long long rlim_t;

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

#define RLIMIT_CORE 4

extern int setrlimit(int __resource, const struct rlimit *__rlimits);

struct rusage {
	long int ru_nvcsw;
	long int ru_nivcsw;
};

#define RUSAGE_THREAD 1

extern int getrusage(int who, struct rusage *__usage);