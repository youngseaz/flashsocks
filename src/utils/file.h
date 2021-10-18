#ifndef _FILE_H
#define _FILE_H

#include<string.h>
#include<sys/stat.h>

#ifndef PATH_MAX
#define PATH_MAX 256
#endif


//#define MODE  0777
//#define UMASK 0022


#define filename(filepath, filename)    \
{                                       \
    int len = strlen(filepath);         \
    int ind = 0;                        \
    while (filepath[--len] != '/');     \
    while (filepath[len++])             \
    {                                   \
        filename[ind++] = filepath[len];\
    }                                   \
    filename[ind] = '\0';               \
}



#define dirname(path, dirname)          \
{                                       \
    int len = strlen(path);             \
    while (path[--len] != '/');         \
    memcpy(dirname, path, len);         \
    dirname[len] = '\0';                \
}



/*
* return:
*   file size in bytes
*/
long getfilesize(const char *filename);


#endif