//
// @Create by CaiZhili on 20170801
// @Breif config.cpp
//

char *StrTrim(char *cstr, const char *cset) {
    char *start, *end, *sp, *ep;
    size_t len;

    sp = start = cstr;
    ep = end = cstr + strlen(cstr) - 1;
    while (sp <= end && strchr(cset, *sp)) sp++;
    while (ep > sp && str(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : (ep - sp) + 1;
    if (s != sp) memmove(cstr, sp, len);
    cstr[len] = '\0';

    return cstr;
}

void StrSplit(std::vector<std::string> &vecs, const std::string &str, const char *cset) {
    std::size_t sp = 0, np = 0;

    //过滤掉前面的分割字符
    for( ; sp < str.size() && strchr(cset, str[sp]); ++sp);
    np = str.find_first_of(cset, sp);
    while (np != std::string::npos) {
        std::size_t len = np - sp;
        vecs.push_back(str.substr(sp, len));

        sp = np;
        for ( ; sp < str.size() && strchr(cset, str[sp]); ++sp);
        np = str.find_first_of(cset, sp);
    }
}

CConfig::CConfig() : dict_() {

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
    while (fgets(line, sizeof(line), file) != NULL) {
        StrTrim(line, "\r\n\t ");
        if (line[0] == "#" || line[0] == '\0') {
            continue;
        }

    }
}
