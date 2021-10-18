#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

 
#include "file.h"

void write_file(const char *file, const char *content, uint32_t size, uint32_t mode)
{
    int fd = open(file, mode);
    write(fd, content, size);
}


inline long getfilesize(const char *filename)
{
    struct stat statbuff;
    if (stat(filename, &statbuff) == -1) return -1;
    return statbuff.st_size;
}


