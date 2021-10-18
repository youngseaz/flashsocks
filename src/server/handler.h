#ifndef HANDLER_H
#define HANDLER_H

#include "socks5.h"

#define EPOLL_SIZE 1024



/*
typedef union epoll_data {         
    void *ptr;          
    int fd;           
    __uint32_t u32;          
    __uint64_t u64;
} epoll_data_t;

struct epoll_event{           
    __uint32_t events;      //* epoll event          
    epoll_data_t data;      //* User data variable
};

*/
typedef struct _socks5_event
{
    int srcfd;
    int dstfd;
}socks5_event_t, *psocks5_event_t;


typedef struct _server_t {
    char ip[16];
    uint16_t port;
    int fd;
} conn_info_t;

typedef struct _conn_ctx {
    conn_info_t client_info;
    conn_info_t remote_info;
} conn_ctx;



void proxy_handler();

#endif