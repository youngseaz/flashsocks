#ifndef _IO_H
#define _IO_H

#ifndef RBUFSIZE
#define RBUFSIZE 16384   // 1024 * 16 = 16KB
#endif


typedef _ring_buffer {
    char rbuffer[RBUFSIZE];
    size_t nleft;
}


void write


int setnonblocking(int fd);

int setfastopen(int fd);

#endif