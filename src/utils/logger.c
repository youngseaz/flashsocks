#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdarg.h>
#include<errno.h>
#include<libgen.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/time.h>


#include "logger.h"

#ifndef PATH_MAX
#define PATH_MAX 256
#endif


const char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
time_t tm_t; 
struct tm *ptm; 

#define getdatetime(datetime) \ 
{\
    time(&tm_t); \
    ptm = gmtime(&tm_t); \
    sprintf(datetime, "%d-%02d-%02d %s %02d:%02d:%02d", \
                    ptm->tm_year + 1900,\
                    ptm->tm_mon + 1, \
                    ptm->tm_mday,\
                    wday[ptm->tm_wday],\
                    ptm->tm_hour,\
                    ptm->tm_min,\
                    ptm->tm_sec);\
} 

// for logger
static const char *DEFAULT_LOGPATH = "/var/log/flashsocks/flashsocks.log";

log_t logi;
                          

/*
* create a file according given filepath
* @filepath: given file path 
* 给定路径，递归创建文件夹及文件
*/

static int createfile(const char *filepath)
{
    char dir[PATH_MAX + 1] = {0};
    logi.packsize = 16777216; // 1024 * 1024 * 16 Bytes, 16MBits
    if (strlen(filepath) > PATH_MAX)
    {
        printf("path too long");
        exit(1);
    }
    strcpy(dir, filepath);
    dirname(dir);

    // if dir doesn't exist
    if (stat(dir, NULL) == -1 && errno == ENOENT)
    {
        // if parent dir exist
        if (createfile(dir) == 1)
        {
            if (mkdir(dir, 0600))
            {
                perror("mkdir fail");
                exit(1);
            }
        }
        
    }
    // return 1 if dir exist
    else
        return 1;

    if (creat(filepath, 0600))
    {
        perror("creat log file fail");
        exit(1);
    }
    return 0;
}


void log_init(int loglevel, const char *logfile)
{
    char buffer[PATH_MAX + 32];
    if (logfile && strlen(logfile) <= PATH_MAX)
        strcpy(logi.logpath, logfile);

    logi.loglevel = loglevel;
        
    // errno is global variable
    if (stat(logi.logpath, NULL) == -1 && errno == ENOENT)
    {
        createfile(logi.logpath);
    }

    logi.fp = fopen(logi.logpath, "a+");
    if (logi.fp == NULL)
    {
        //fprintf(stderr, "Initial logger fail: can't open %s!\n", logi.logpath);
        sprintf(buffer, "Initial logger fail: %s", logi.logpath);
        perror(buffer);
        exit(1);
    }
}


void logger(int level, 
            const char *file,
            int line,
            const char *func,
            const char *fmt, ...)
{
    static char buf[256];
    char datetime[32];
    
    if (logi.loglevel < LEVEL_DEBUG || logi.loglevel > LEVEL_CRITICAL)
    {
        fprintf(stderr, "invalid log level\n");
        exit(1);
    }
    switch(level)
    {
        case LEVEL_DEBUG:
            if (logi.loglevel > LEVEL_DEBUG)
                return;
            getdatetime(datetime);
            fprintf(stdout, "%s [debug] %s:%d In function '%s' --> ", datetime, file, line, func);
            break;
        case LEVEL_INFO:
            if (logi.loglevel > LEVEL_INFO)
                return;
            getdatetime(datetime);
            fprintf(stdout, "%s [info] %s:%d In function '%s' --> ", datetime, file, line, func);
            break;
        case LEVEL_WARNING:
            if (logi.loglevel > LEVEL_WARNING)
                return;
            getdatetime(datetime);
            fprintf(stdout, "%s [warning] %s:%d In function '%s' --> ", datetime, file, line, func);
            break;
        case LEVEL_ERROR:
            if (logi.loglevel > LEVEL_ERROR)
                return;
            getdatetime(datetime);
            fprintf(stdout, "%s [error] %s:%d In function '%s' --> ", datetime, file, line, func);
            break;
        case LEVEL_CRITICAL:
            getdatetime(datetime);
            fprintf(stdout, "%s [critical] %s:%d In function '%s' --> ", datetime, file, line, func);
            break;
    }
    va_list vaList;
	va_start(vaList, fmt);
	vfprintf(stdout, fmt, vaList);
	va_end(vaList);
	fprintf(stdout, "%c", '\n');
}


static void packlog()
{
    return;
}
