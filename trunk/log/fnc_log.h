#ifndef FNC_LOGH
#define FNC_LOGH

#define FNC_LOG_OUT 0 
#define FNC_LOG_SYS 1

//level
#define FNC_LOG_FATAL 0 
#define FNC_LOG_ERR 1
#define FNC_LOG_WARN 2 
#define FNC_LOG_INFO 3 
#define FNC_LOG_DEBUG 4 
#define FNC_LOG_VERBOSE 5 
#define FNC_LOG_CLIENT 6 

void fnc_init(int level);
void fnc_log(int level, const char *fmt, ...);

#endif
