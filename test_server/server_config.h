#ifndef NF_TEST_SERVER_CONFIG_H
#define NF_TEST_SERVER_CONFIG_H

#include <string>

#include "../src/singleton.h"

struct ServerConfig {
public:
    int Init(const char *cfg_file);

public:
    int port;
    std::string ipv4;
    std::string ipv6;
    int out_time;

    int deamon;
};

typedef nf::singleton<ServerConfig> svr_cfg;

#endif //NF_TEST_SERVER_CONFIG_H
