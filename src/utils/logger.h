#ifndef _LOGGER_H
#define _LOGGER_H

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

typedef struct _log
{
	int loglevel;
	FILE *fp;
	size_t packsize;
	size_t logsize; 
	char logpath[PATH_MAX + 1];
}log_t;


// four log level
#define LEVEL_DEBUG    0x00
#define LEVEL_INFO     0x01
#define LEVEL_WARNING  0x02
#define LEVEL_ERROR    0x03
#define LEVEL_CRITICAL 0x04

void logger(int level, const char *file, int line, const char *func, const char *fmt, ...);

#define LOG_DEBUG(...)	  logger(LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)     logger(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARNING(...)  logger(LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...)    logger(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_CRITICAL(...) logger(LEVEL_CRITICAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif