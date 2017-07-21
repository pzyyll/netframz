/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#include "server.h"
#include "server_config.h"

TServer::TServer() : conf_file_(NULL), accept_task_(NULL) {

}

TServer::~Tserver() {

}

int TServer::Init(int argc, char **argv) {
    if (GetOption() < 0) {
        return FAIL;
    }

    //Set server listen
    //1. create socket fd
    //2. bind port and addr
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        return FAIL;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htonl(svr_cfg.get_const_instance().port);
    if (svr_cfg.get_const_instance().ipv4 != "") {
        inet_pton(AF_INET, svr_cfg.get_const_instance().ipv4.c_str(), &addr.sin_addr);
    } else {
        addr.sin_addr = INADDR_ANY;
    }

    //3. drive fd to listen
    //todo first
}

//=====================private===========================//
int GetOption(int argc, char **argv) {
    int ret = 0;
    struct option ops[] = {
        {"help", no_argument, 0, 'h'},
        {"conf", required_argument, 0, 'c'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    int c = 0, ops_idx = 0;
    while ((c = getopt_long(argc, argv, "hc:v", ops, &ops_idx) ) != -1) {
        switch (c):
        case 'h':
            printf("Unfortunately, there is no any help.")
            ret = -1;
            break;
        case 'c':
            //set conf file to server_config
            conf_file = optarg;
            break;
        case 'v':
            printf("\tTest server v1.\n");
            printf("\tDate 2017/7/21.");
            ret = -1;
            break;
        case '?':
            ret = -1;
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}
