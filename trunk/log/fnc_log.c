#include <stdio.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include "fnc_log.h"

#define LOG_FORMAT "%d/%b/%Y:%H:%M:%S %z"
#define ERR_FORMAT "%a %b %d %H:%M:%S %Y"
#define MAX_LEN_DATE 30 

static int Print_Level = FNC_LOG_FATAL;

void fnc_init(int level)
{
	Print_Level = level;
}

/*Prints on standard error or file*/
void fnc_log(int level, const char *fmt, ...)
{
	if(level > Print_Level)
	{
		return;
	}
	
    va_list args;
    time_t now;
    char date[MAX_LEN_DATE];
    int no_print=0;
    const struct tm *tm;

    time(&now);
    tm=localtime(&now);

    switch (level)
    {
        case FNC_LOG_FATAL:
            strftime(date, MAX_LEN_DATE, ERR_FORMAT ,tm);
            fprintf(stderr, "[%s] [fatal error] ",date);
            break;
            
        case FNC_LOG_ERR:
            strftime(date, MAX_LEN_DATE, ERR_FORMAT ,tm);
            fprintf(stderr, "[%s] [error] ",date);
            break;
            
        case FNC_LOG_WARN:
            strftime(date, MAX_LEN_DATE, ERR_FORMAT ,tm);
            fprintf(stderr, "[%s] [warning] ",date);
            break;
            
        case FNC_LOG_DEBUG:
            no_print=1;	 
            break;
            
        case FNC_LOG_VERBOSE:
            no_print=1;
            break;
            
        case FNC_LOG_CLIENT:
            break;
            
        default:
            /*FNC_LOG_INFO*/
            strftime(date, MAX_LEN_DATE, LOG_FORMAT ,tm);
            fprintf(stderr, "[%s] ",date);
            break;
    }
	
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stderr);
		
}