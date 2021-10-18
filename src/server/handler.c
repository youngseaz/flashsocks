#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdint.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "handler.h"
#include "socks5.h"
#include "../third-pard/sockets/rdwrn.h"
#include "../utils/logger.h"

#ifndef EPOLL_SIZE
#define EPOLL_SIZE 4096
#endif

#ifndef MAX_EVENTS
#define MAX_EVENTS 100
#endif

#ifndef EPOLL_TIMEOUT
#define EPOLL_TIMEOUT 0
#endif


/*
* 
* read data from srcfd and then write to dstfd
*
*/
static void socks5_tcp_relay(int srcfd, int dstfd) {
    int n;
    n = read(srcfd, socks5_buf, SOCKS5_BUFSIZE);
    if (n < 0 && errno == ECONNRESET) {
        LOG_ERROR("read error");
    }
    n = writen(dstfd, socks5_buf, n);
    if (n < 0 && errno == EPIPE) {
        LOG_ERROR("write error");
    }
    return n;
}


/*
typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;

struct epoll_event {
    __uint32_t events;      // Epoll events 
    epoll_data_t data;      // User data variable 
};
*/


static void epoll_add(int epfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    /*
    int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    @op:
        EPOLL_CTL_ADD    // add new fd to epfd
        EPOLL_CTL_MOD    // modify the events of registed fd
        EPOLL_CTL_DEL    // delete fd from epfd
    @fd:
        the socket you want to listen
    @event:
        the events you want to listen
    */
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}


static void epoll_del(int epfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    /*
    int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    @op:
        EPOLL_CTL_ADD    // add new fd to epfd
        EPOLL_CTL_MOD    // modify the events of registed fd
        EPOLL_CTL_DEL    // delete fd from epfd
    @fd:
        the socket you want to listen
    @event:
        the events you want to listen
    */
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
}


static int setnonblocking(int fd)
{
    int ret, flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) 
    {
        LOG_ERROR("get fd status failed: %s", strerror(errno));
        return -1;
    }

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1) 
    {
        LOG_ERROR("set fd status failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}



void tcp_handler()
{
    int ret, i, n = 0;
    int client_fd, remote_fd;
    size_t nfds, epfd;
    struct epoll_event ev, event[MAX_EVENTS];

    socks5_event_t *pairs;

    uint32_t events;

    int listenfd = socks5_init();

    epfd = epoll_create(EPOLL_SIZE);
    if (epfd == -1)
    {
        LOG_CRITICAL("epoll_create error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // 数据到来，边缘触发
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    if (ret == -1)
    {
        LOG_CRITICAL("epoll_ctl error: %s", strerror(errno));
        close(listenfd);
        exit(EXIT_FAILURE);
    }
    while (true)
    {
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        nfds = epoll_wait(epfd, event, MAX_EVENTS, EPOLL_TIMEOUT);
        if (ret == -1)
        {
            LOG_ERROR("epoll_wait error: %s", strerror(errno));
            //exit(EXIT_FAILURE);
            continue;
        }
        for (i = 0; i < nfds; i++)
        {
            events = event[i].events;
            if ( events & EPOLLERR || events & EPOLLHUP || (! events & EPOLLIN)) 
            {
                LOG_ERROR("poll has error");
                pairs = (socks5_event_t *)event[i].data.ptr;
                close(pairs->srcfd);
                continue;
            }
            else if (listenfd == event[i].data.fd)
            {
                while (true)
                {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    memset(&client_addr, 0, client_addr_len);
                    client_fd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (client_fd == -1)
                    {
                        //LOG_ERROR("accept error: %s", strerror(errno));
                        LOG_ERROR("accept error!");
                        break;
                    }
                    LOG_INFO("accept IP %s", inet_ntoa(client_addr.sin_addr));
                    remote_fd = socks5_shakehands(client_fd);
                    if (remote_fd == -1) {
                        continue;
                    }
                    setnonblocking(client_fd);
                    setnonblocking(remote_fd);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.ptr = malloc(sizeof(socks5_event_t));
                    pairs = (socks5_event_t *)ev.data.ptr;
                    pairs->srcfd = client_fd;
                    pairs->dstfd = remote_fd;
                    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                    if (ret == -1)
                    {
                        LOG_CRITICAL("epoll_ctl error: %s", strerror(errno));
                        continue;
                    }
                    ev.data.ptr = malloc(sizeof(socks5_event_t));
                    pairs = (socks5_event_t *)ev.data.ptr;
                    pairs->srcfd = remote_fd;
                    pairs->dstfd = client_fd;
                    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, remote_fd, &ev);
                    if (ret == -1) {
                        LOG_CRITICAL("epoll_ctl error: %s", strerror(errno));
                        continue;
                    }
                }
            }
            else {
                LOG_DEBUG("TEST: %s", strerror(errno));
                pairs = (socks5_event_t *)event[i].data.ptr;
                socks5_tcp_relay(pairs->srcfd, pairs->dstfd);
            }   
        }
    }
}


void proxy_handler() {
    tcp_handler();
}