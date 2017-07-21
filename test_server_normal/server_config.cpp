#include "server_config.h"

int ServerConfig::Init(const char *cfg_file) {
    //todo add cfg_file

    port = 23399;
    ipv4 = "";
    ipv6 = "";
    timeout = 1500;
    deamon = 0;

    return 0;
}
