#ifndef NF_TEST_SERVER_CONFIG_H
#define NF_TEST_SERVER_CONFIG_H

#include <string>

#include "singleton.h"

struct ServerConfig {
public:
    int Init(const char *cfg_file);

public:
    unsigned int port;
    std::string ipv4;
    std::string ipv6;
    unsigned int timeout;
    unsigned int deamon;
    unsigned int log_level;
};

typedef nf::singleton <ServerConfig> svr_cfg;

#endif //NF_TEST_SERVER_CONFIG_H
