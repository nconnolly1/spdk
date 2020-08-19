#include <winsock2.h>
#include <ws2tcpip.h>

//HACK - only works on build 17063 onwards
#include <afunix.h>

#if 0
struct sockaddr {
    char sa_data[14];
};
#endif

typedef int socklen_t;

struct msghdr {
    void *msg_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    size_t msg_iovlen;
    void *msg_control;
    size_t msg_controllen;
    int msg_flags;
};

#define	IFNAMSIZ 16
struct ifreq {
	char ifr_name[IFNAMSIZ];
	struct	sockaddr ifr_addr;
	short ifr_flags;
};

extern ssize_t sendmsg(int __fd, const struct msghdr *__message, int __flags);

struct ifaddrs {
  struct ifaddrs *ifa_next;
  char *ifa_name;
  unsigned int ifa_flags;
  struct sockaddr *ifa_addr;
};

extern int getifaddrs(struct ifaddrs **__ifap);
extern void freeifaddrs(struct ifaddrs *__ifa);

#define SIOCGIFFLAGS 1