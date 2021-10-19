#ifndef _FILE_H
#define _FILE_H

#include<string.h>
#include<sys/stat.h>

#ifndef PATH_MAX
#define PATH_MAX 256
#endif


//#define MODE  0777
//#define UMASK 0022


/*
* return:
*   file size in bytes
*/
long getfilesize(const char *filename);


#endif