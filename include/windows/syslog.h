extern void openlog(const char *__ident, int __option, int __facility);
extern void closelog(void);
extern void syslog(int __pri, const char *__fmt, ...);

#define LOG_INFO 1
#define LOG_ERR 2
#define LOG_WARNING 4
#define LOG_NOTICE 5

#define LOG_PID 1
#define LOG_LOCAL7 1
