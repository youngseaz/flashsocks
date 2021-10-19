#include<stdio.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

#include "logger.h"

/* 
 * set fd nonblocking
 * @fd: file descripto
 * @return:
        return 0 in success
        return -1 in failure
 */

int setnonblocking(int fd) {
    int ret, flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        LOG_ERROR("get fd status failed: %s", strerror(errno));
        return -1;
    }

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1) {
        LOG_ERROR("set fd status failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}




int setfastopen(int fd) {
    
#ifdef FAST_OPEN
    int qlen=5;
    setsockopt(serverSock, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));
#endif
}