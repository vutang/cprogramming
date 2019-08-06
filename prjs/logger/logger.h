#ifndef RRU_LOG_H_
#define RRU_LOG_H_

#define DEBUG	 	0
#define INFO 		1
#define WARN  		2
#define ERROR 		3
#define FATAL 		4

#define LOG_TO_SCREEN    2
#define LOG_TO_FILE     1

int config_log(char * path, unsigned int filter, unsigned mode);
void logger(int level, char *fname, int line_number, const char *format,...);

#define LOG_WARN(msg,...) logger(WARN, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_DEBUG(msg,...) logger(DEBUG, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_INFO(msg,...) logger(INFO, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg,...) logger(ERROR, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg,...) logger(FATAL, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#endif /* RRU_LOG_H_ */