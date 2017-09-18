#ifndef NF_TESTSVR_NOR_H
#define NF_TESTSVR_NOR_H

#include <syslog.h>

extern int syslog_level;

#define LOG_LV_DEBUG 4
#define LOG_LV_INFO 3
#define LOG_LV_WARN 2
#define LOG_LV_ERR 1

#define log_init(name, level) \
    do { \
        syslog_level = level; \
        openlog(name, LOG_NDELAY|LOG_PID, LOG_USER); \
    } while (0)

#define log_close() closelog()

#define log_pet(level, dlv, format, args ...) \
    do { \
        if (syslog_level >= level) \
            syslog(dlv, "<%s:%d:%s> " format, __FILE__, __LINE__, __FUNCTION__, ##args); \
    } while (0)

#define log_debug(str, args ...) \
    do { \
        log_pet(LOG_LV_DEBUG, LOG_DEBUG, str, ##args); \
    } while (0)

#define log_info(str, args ...) \
    do { \
        log_pet(LOG_LV_INFO, LOG_INFO, str, ##args); \
    } while (0)

#define log_warn(str, args ...) \
    do { \
        log_pet(LOG_LV_WARN, LOG_WARNING, str, ##args); \
    } while (0)

#define log_err(str, args ...) \
    do { \
        log_pet(LOG_LV_ERR, LOG_ERR, str, ##args); \
    } while (0)

#endif //NF_TESTSVR_NOR_H

