#ifndef _RDWRN_H
#define _RDWRN_H


ssize_t readn(int fd, void *buffer, size_t n);

ssize_t writen(int fd, const void *buffer, size_t n);

#endif