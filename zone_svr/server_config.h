#ifndef NF_TEST_SERVER_CONFIG_H
#define NF_TEST_SERVER_CONFIG_H

#include <string>

#include "config.h"
#include "singleton.h"

struct ServerConfig {
public:
    int Init(const char *cfg_file);

public:
    int         port;
    std::string ipv4;
    //std::string ipv6;
    int         tick;
    int         timeout;
    int         daemon;
    int         max_fds;
    int         log_level;
};

typedef nf::singleton <ServerConfig> svr_cfg;

#endif //NF_TEST_SERVER_CONFIG_H
