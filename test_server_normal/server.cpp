/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#include <fcntl.h>

#include "server.h"
#include "server_config.h"
#include "log.h"

using namespace std::placeholders;
using namespace std;

TServer::TServer() : conf_file_(NULL), accept_task_(NULL), tick_(NULL) {

}

TServer::~Tserver() {

}

int TServer::Init(int argc, char **argv) {
    if (GetOption() < 0) {
        return FAIL;
    }

    if (svr_cfg.get_mutable_instance().Init(conf_file_) < 0) {
        return FAIL;
    }

    log_init(argv[0], svr_cfg.get_const_instance().log_level);

    if (StartListen() < 0) {
        return FAIL;
    }

    if (StartTick() < 0) {
        return FAIL;
    }

    //todo first
}

int TServer::StartListen() {
    //Set server listen
    //1. create socket fd
    //2. bind port and addr
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        log_err("Socket Fail. %s", strerror(errno));
        return FAIL;
    }

    if (MakeNonblock(listen_fd) < 0) {
        return FAIL;
    }

    int reuse = 1;
    if (::setsockopt(listen_fd, SOL_SOCKET, SOREUSEADDR, &reuse, sizeof(reuse)) < 0) {
        log_err("Setsockopt for fd(%d) to reuseaddr fail. ",
                listen_fd, strerror(errno));
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

    if (::bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        log_err("Bind for listen fd(%d) fail. %s", listen_fd, strerror(errno));
        return FAIL;
    }

    //3. drive fd to listen
    if (::listen(listen_fd, 4096)) {
        log_err("listen fd(%d) fail. %s", strerror(errno));
        return FAIL;
    }

    accept_task_ = new IOTask(loop_, listen_fd, EV_READABLE);
    if (NULL == accept_task_) {
        log_err("New fail.");
        return FAIL;
    }
    accept_task_->Bind(std::bind(&TServer::OnAccept, this, _1, _2, _3));
    task_data_t data = {.data = {.ptr = accept_task_}};
    accept_task_->SetPrivateData(data);
    accept_task_->Start();

    log_debug("Listen start.");
    return listen_fd;
}

int TServer::StartTick() {
    tick_ = new TimerTask(loop_, 100, 1);
    if (NULL == tick_) {
        log_err("New fail.");
        return FAIL;
    }
    tick_->Bind(std::bind(&TServer::OnTick, this, _1, _2, _3));
    task_data_t pridata = {.data = {.ptr = tick_}};
    tick_->SetPrivateData(pridata);

    tick_->Start();

    log_debug("Tick start.");
    return SUCCESS;
}

void TServer::OnAccept(EventLoop *loopsv, task_data_t data, int mask) {
    log_debug("OnAccept.");
    int listen_fd = accept_task_->GetFd();

    while (true) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        int fd = ::accept(listen_fd, (struct sockaddr *) &addr, sizeof(addr));
        if (fd < 0) {
            if (EAGAIN == errno)
                break;
            log_warn("Accept fail.");
            break;
        }

        if (MakeNonblock(fd) < 0) {
            return;
        }

        if (SetCliOpt(fd) < 0) {
            return;
        }

        ConnectorPtr cli = new Connector(loop_, fd);
        unsigned long cid = cli->GetCID();
        if (cli == NULL | con_map_.insert(std::make_pair(cid, cli)).second == false) {
            log_err("Add cli connector to map fail, fd|%d", fd);
            return;
        }
        IOTask &task = cli->GetIOTask();
        task.Bind(std::bind(&TServer::OnRead, this, _1, _2, _3));
        task_data_t pridata = {.data = {.id = cid}};
        task.SetPrivateData(pridata);
        task.Start();

    }
}

//=====================private===========================//
int TServer::GetOption(int argc, char **argv) {
    int ret = 0;
    struct option ops[] = {
            {"help",    no_argument,       0, 'h'},
            {"conf",    required_argument, 0, 'c'},
            {"version", no_argument,       0, 'v'},
            {0, 0,                         0, 0}
    };

    int c = 0, ops_idx = 0;
    while ((c = getopt_long(argc, argv, "hc:v", ops, &ops_idx)) != -1) {
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

int TServer::MakeNonblock(int fd) {
    int val = 0;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        log_warn("Get fd(%d) stat fail. %s", fd, strerror(errno));
    }

    val |= O_NONBLOCK;
    if (::fcntl(fd, F_SETFL, val) < 0) {
        log_warn("Set fd(%d) non-block fail. %s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int TServer::SetCliOpt(int fd) {
    //todo first
    int send_buff_size = 4 * 32768;
    int recv_buff_size = 4 * 32768;

    if (::setsockopt(fd, SOL_SOCKET, SO_SNDBUFF,
                     (void *) &send_buff_size, sizeof(send_buff_size)) < 0)

}
