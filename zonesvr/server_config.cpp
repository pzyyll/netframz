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
    cfg.GetInt(daemon, "daemon", 0);
    cfg.GetInt(max_fds, "max_fds", 65536);
    cfg.GetInt(syslog_level, "syslog_level", 2);
    cfg.GetInt(zone_num, "zone_num", 10);
    cfg.GetInt(log_level, "log_level", 0);
    cfg.GetString(log_path, "log_path", "./");

    return 0;
}
