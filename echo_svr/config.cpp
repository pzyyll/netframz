//
// @Create by CaiZhili on 20170801
// @Breif config.cpp
//

#include <iostream>
#include "config.h"

char *StrTrim(char *cstr, const char *cset) {
    char *start, *end, *sp, *ep;
    std::size_t len;

    sp = start = cstr;
    ep = end = cstr + strlen(cstr) - 1;
    while (sp <= end && strchr(cset, *sp)) sp++;
    while (ep > sp && strchr(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : (ep - sp) + 1;
    if (cstr != sp) memmove(cstr, sp, len);
    cstr[len] = '\0';

    return cstr;
}

void StrSplit(std::vector<std::string> &vecs, const std::string &str, const char *cset) {
    std::size_t sp = 0, np = 0;

    //过滤掉前面的分割字符
    while(sp < str.size() && strchr(cset, str[sp])) ++sp;
    np = str.find_first_of(cset, sp);
    while (np != std::string::npos) {
        std::size_t len = np - sp;
        vecs.push_back(str.substr(sp, len));

        sp = np;
        while(sp < str.size() && strchr(cset, str[sp])) ++sp;
        np = str.find_first_of(cset, sp);
    }

    if (sp < str.size())
        vecs.push_back(str.substr(sp));
}

CConfig::CConfig() {

}

CConfig::CConfig(const char *path) {
    Init(path);
}

CConfig::~CConfig() {

}

int CConfig::Init(const char *path) {
    FILE *fp = fopen(path, "r");

    if (NULL == fp) {
        snprintf(err_msg_, sizeof(err_msg_), "Open file fail. %s", strerror(errno));
        return FAIL;
    }

    char line[MAX_LINE_SIZE + 1];
    while (fgets(line, sizeof(line), fp) != NULL) {
        StrTrim(line, "\r\n\t ");
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }
        std::vector<std::string> vecs;
        StrSplit(vecs, std::string(line), "\r\n\t ");
        if (vecs.size() == 0)
            continue;
        dict_.insert(std::make_pair(vecs[0], std::string(line)));
    }
    return SUCCESS;
}

void CConfig::GetString(std::string &val, const char *name, std::string default_val) {
    val = default_val;

    dict_t::iterator find = dict_.find(name);
    if (find == dict_.end())
        return;

    std::vector<std::string> strvec;
    StrSplit(strvec, find->second, "\t ");
    if (strvec.size() < 2) {
        return;
    }

    val = strvec[1];
}

void CConfig::GetInt(int &val, const char *name, int default_val) {
    char cstr_int[32];
    snprintf(cstr_int, sizeof(cstr_int), "%d", default_val);
    std::string strval;
    GetString(strval, name, std::string(cstr_int));
    val = atoi(strval.c_str());
}

void CConfig::GetULL(unsigned long long &val, const char *name, unsigned long long default_val) {
    char cstr_ull[32];
    snprintf(cstr_ull, sizeof(cstr_ull), "%llu", default_val);
    std::string strval;
    GetString(strval, name, std::string(cstr_ull));
    val = strtoull(strval.c_str(), NULL, 10);
}

void CConfig::GetStringArray(std::vector<std::string> &strvec, const char *name) {
    strvec.clear();
    dict_t::iterator find = dict_.find(name);
    if (find == dict_.end()) {
        return;
    }

    StrSplit(strvec, find->second, "\t ");
    if (strvec.size() == 0) {
        return;
    }

    strvec.erase(strvec.begin());
}

void CConfig::GetIntArray(std::vector<int> &ivec, const char *name) {
    ivec.clear();
    std::vector<std::string> strvec;
    GetStringArray(strvec, name);
    for (unsigned int i = 0; i < strvec.size(); ++i) {
        int num = atoi(strvec[i].c_str());
        ivec.push_back(num);
    }
}

void CConfig::GetULLArray(std::vector<unsigned long long> &ullvec, const char *name) {
    ullvec.clear();
    std::vector<std::string> strvec;
    GetStringArray(strvec, name);
    for (unsigned int i = 0; i < strvec.size(); ++i) {
        unsigned long long num = strtoull(strvec[i].c_str(), NULL, 10);
        ullvec.push_back(num);
    }
}
