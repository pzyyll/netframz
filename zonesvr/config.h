//
// @Create by CaiZhili on 20170801
// @Breif config.h
//

#ifndef NF_ECHO_SVR_CONFIG_H
#define NF_ECHO_SVR_CONFIG_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#define MAX_LINE_SIZE 1024

class CConfig {
public:
    enum RET {
        FAIL    = -1,
        SUCCESS = 0,
    };

public:
    CConfig();
    CConfig(const char *path);
    ~CConfig();

    int Init(const char *path);

    void GetString(std::string &val, const char *name, std::string default_val = "");

    void GetInt(int &val, const char *name, int default_val = 0);

    void GetULL(unsigned long long &val, const char *name, unsigned long long default_val = 0);

    void GetStringArray(std::vector<std::string> &strvec, const char *name);

    void GetIntArray(std::vector<int> &ivec, const char *name);

    void GetULLArray(std::vector<unsigned long long> &ullvec, const char *name);

    char *get_err_msg();

private:
    typedef std::map<std::string, std::string> dict_t;
    dict_t dict_;
    char   err_msg_[256];
};

#endif //NF_ECHO_SVR_CONFIG_H
