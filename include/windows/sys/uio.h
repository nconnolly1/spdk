#include <stddef.h>

struct iovec {
    void *iov_base;
    size_t iov_len;
};

extern ssize_t readv(int __fd, const struct iovec *__iovec, int __count);
extern ssize_t writev(int __fd, const struct iovec *__iovec, int __count);