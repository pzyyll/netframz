#include "server_config.h"

int ServerConfig::Init(const char *cfg_file) {
    //todo add cfg_file

    port = 23399;
    ipv4 = "";
    out_time = 1500;
    deamon = 0;
}
