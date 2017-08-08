#include <iostream>
#include "server_config.h"

int ServerConfig::Init(const char *cfg_file) {
    if (NULL == cfg_file) {
        return -1;
    }

    CConfig cfg(cfg_file);

    cfg.GetInt(port, "port", 23399);
    cfg.GetString(ipv4, "ipv4", "");
    cfg.GetInt(tick, "tick", 100);
    cfg.GetInt(timeout, "timeout", 30000);
    cfg.GetInt(worker, "worker", 1);
    cfg.GetInt(daemon, "daemon", 0);
    cfg.GetInt(log_level, "log_level", 2);

    return 0;
}
