#ifndef _IO_H
#define _IO_H

#ifndef BUFSIZE
#define BUFSIZE 4096
#endif


int setnonblocking(int fd);

int setfastopen(int fd);

#endif