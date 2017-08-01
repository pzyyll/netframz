#ifndef NF_ECHO_SVR_CONFIG_H
#define NF_ECHO_SVR_CONFIG_H

#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#define MAX_LINE_SIZE 1024;

class CConfig {
public:
    enum {
        FAIL = -1,
        SUCCESS = 0,
    }

public:
    CConfig();
    ~CConfig();

    int Init(const char *path);

    char *get_err_msg();

private:
    std::map<std::string, std::string> dict_;
    char err_msg_[256];
}

#endif //NF_ECHO_SVR_CONFIG_H
