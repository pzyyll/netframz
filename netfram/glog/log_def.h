//
// @Created by czllo on 2017/9/10.
// @Brief
//

#ifndef SERVER_LOG_DEF_H
#define SERVER_LOG_DEF_H

#include <iostream>
#include <string>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <sys/errno.h>

#include "logging.h"

#ifdef LOG_DEF

inline void iniglog(const char *cstrArgv0) {
    google::InitGoogleLogging(cstrArgv0);
}

inline void deiniglog() {
    google::ShutdownGoogleLogging();
}

inline void set_logbufsecs(int secs) {
    FLAGS_logbufsecs = secs;
}

inline void set_minloglevel(int level) {
    FLAGS_minloglevel = level;
}

inline void set_log_infopath(const char *cstrPath) {
    google::SetLogDestination(google::GLOG_INFO, cstrPath);
}
inline void set_log_warnpath(const char *cstrPath) {
    google::SetLogDestination(google::GLOG_WARNING, cstrPath);
}
inline void set_log_errorpath(const char *cstrPath) {
    google::SetLogDestination(google::GLOG_ERROR, cstrPath);
}

inline int InitLog(const char *name, int level, const char *path) {
    int ret = 0;
    ret = mkdir(path, S_IRWXU);
    if (ret < 0 && errno == ENOENT) {
        switch (errno) {
            case EEXIST:
                ret = 0;
                break;
            default:
                std::cout << "log path not exit!" << std::endl;
                return ret;
        }
    }

    std::string base_path(path);
    if (base_path.back() != '/')
        base_path.push_back('/');
    std::string info_path = base_path + "info/";
    std::string warn_path = base_path + "warn/";
    std::string err_path  = base_path + "error/";

    mkdir(info_path.c_str(), S_IRWXU);
    mkdir(warn_path.c_str(), S_IRWXU);
    mkdir(err_path.c_str(), S_IRWXU);

    info_path.append("log_");
    warn_path.append("log_");
    err_path.append("log_");

    iniglog(name);
    set_log_infopath(info_path.c_str());
    set_log_warnpath(warn_path.c_str());
    set_log_errorpath(err_path.c_str());
    set_logbufsecs(0);
    set_minloglevel(level);

    return ret;
}

#define LogInfo(str, args...) \
    do {    \
        char buf[2048] = {0};    \
        snprintf(buf, sizeof(buf), str, ##args);    \
        LOG(INFO) << "<" << __FUNCTION__ << "> " << buf; \
    } while (0)

#define LogWarn(str, args...) \
    do { \
        char buf[2048] = {0}; \
        snprintf(buf, sizeof(buf), str, ##args); \
        LOG(WARNING) << "<" << __FUNCTION__ << "> " << buf; \
    } while (0)

#define LogErr(str, args...) \
    do {  \
        char buf[2048] = {0}; \
        snprintf(buf, sizeof(buf), str, ##args); \
        LOG(ERROR) << "<" << __FUNCTION__ << "> " << buf; \
    } while (0);

#else

inline int InitLog(const char *name, int level, const char *path) {
    return 0;
}
inline void iniglog(const char *cstrArgv0) {

}

inline void deiniglog() {

}

inline void set_logbufsecs(int secs) {

}

inline void set_minloglevel(int level) {

}

inline void set_log_infopath(const char *cstrPath) {

}
inline void set_log_warnpath(const char *cstrPath) {

}
inline void set_log_errorpath(const char *cstrPath) {

}

#define LogInfo(str, args ...)

#define LogWarn(str, args ...)

#define LogErr(str, args ...)

#endif //LOG_DEF

#endif //SERVER_LOG_DEF_H
