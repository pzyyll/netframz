/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#include "server.h"
#include "server_config.h"
#include "log.h"
#include "mem_check.h"
#include "proto.h"

#define UNUSE(x) ((void)(x))

using namespace std::placeholders;
using namespace std;
using namespace proto;

TServer::TServer() : conf_file_(NULL), accept_task_(NULL), tick_(NULL) {

}

TServer::~TServer() {
    if (accept_task_)
        delete accept_task_;
    if (tick_)
        delete tick_;
}

int TServer::Init(int argc, char **argv) {
    if (GetOption(argc, argv) < 0) {
        return FAIL;
    }

    if (svr_cfg::get_mutable_instance().Init(conf_file_) < 0) {
        return FAIL;
    }

    log_init(argv[0], svr_cfg::get_const_instance().log_level);

    if (StartListen() < 0) {
        return FAIL;
    }

    if (StartTick() < 0) {
        return FAIL;
    }

    return SUCCESS;
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
    if (::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        log_err("Setsockopt for fd(%d) to reuseaddr fail. %s",
                listen_fd, strerror(errno));
        return FAIL;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(svr_cfg::get_const_instance().port);
    if (svr_cfg::get_const_instance().ipv4 != "") {
        log_debug("Set ip addr %s.", svr_cfg::get_const_instance().ipv4.c_str());
        inet_pton(AF_INET, svr_cfg::get_const_instance().ipv4.c_str(), &addr.sin_addr);
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }

    log_debug("Listen port = %u|%u, addr = %s.",
              svr_cfg::get_const_instance().port, addr.sin_port,
              svr_cfg::get_const_instance().ipv4.c_str());

    if (::bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        log_err("Bind for listen fd(%d) fail. %s", listen_fd, strerror(errno));
        return FAIL;
    }

    //3. drive fd to listen
    if (::listen(listen_fd, 4096) < 0) {
        log_err("listen fd(%d) fail. %s", listen_fd, strerror(errno));
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
    UNUSE(loopsv);
    UNUSE(data);
    UNUSE(mask);
    int listen_fd = accept_task_->GetFd();

    while (true) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);
        int fd = ::accept(listen_fd, (struct sockaddr *) &addr, &len);
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

        ConnectorPtr conn = CreateConn(fd);
        if (!conn) {
            ::close(fd);
            return;
        }

        if (!CreateTimerTask(conn->GetCID())) {
            ::close(fd);
            return;
        }

        struct ConnCbData cb_data;
        cb_data.pri_data.data.id = conn->GetCID();
        cb_data.handler = std::bind(&TServer::OnRead, this, _1, _2, _3);
        conn->BeginRecv(cb_data);
    }
}

void TServer::OnRead(unsigned long lenth, task_data_t data, ErrCode err) {
    unsigned long cid = data.data.id;
    if (err.get_ret() != ErrCode::SUCCESS) {
        log_info("%s", err.get_err_msg().c_str());
        DelConn(cid);
        DelTimerTask(cid);
        return;
    }

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return;
    }

    //Do
    Do(*conn);
}

static char buff[1024 * 1024];
void TServer::Do(Connector &conn) {
    log_debug("Do");

    conn.SetLastActTimeToNow();
    unsigned long nr = conn.Recv(buff, sizeof(buff));

    log_debug("rv: %lu", nr);
    std::vector<std::string> vec_strs;
    Cmd::Unpack(vec_strs, buff, nr);

    // echo
    std::string sndstr;
    for (int i = 0; (unsigned int)i < vec_strs.size(); ++i) {
        Cmd::Packing(sndstr, vec_strs[i]);
    }

    log_debug("snd: %li", (long int)sndstr.size());

    struct ConnCbData cbdata;
    cbdata.pri_data.data.id = conn.GetCID();
    cbdata.handler = std::bind(&TServer::OnWriteErr, this,_1, _2, _3);
    conn.Send(sndstr.c_str(), sndstr.size(), cbdata);
}

void TServer::OnWriteErr(unsigned long lenth, task_data_t data, ErrCode err) {
    UNUSE(lenth);
    unsigned long cid = data.data.id;
    if (err.get_ret() != ErrCode::SUCCESS) {
        log_warn("%s", err.get_err_msg().c_str());
        DelConn(cid);
        DelTimerTask(cid);
        return;
    }
}

void TServer::OnTick(EventLoop *loopsv, task_data_t data, int mask) {
    //log_debug("OnTick");
    UNUSE(loopsv);
    UNUSE(data);
    UNUSE(mask);

    //Do some need tick task;
}

void TServer::OnTimerOut(EventLoop *loopsv, task_data_t data, int mask) {
    log_debug("OnTimerOut.");
    UNUSE(loopsv);
    UNUSE(mask);

    unsigned long cid = data.data.id;

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Connector for cid %lu not find.", cid);
        DelTimerTask(cid);
        return;
    }

    if (conn->IsTimeOut(svr_cfg::get_const_instance().timeout / 1000)) {
        log_debug("Close cid %lu this long time not act.", cid);
        DelConn(conn);
        DelTimerTask(cid);
        return;
    }

    log_debug("Resume timer.");
    TimerTaskPtr timer = FindTimer(cid);
    timer->Restart();
}

void TServer::Run() {
    loop_.Run();
}

void TServer::Stop() {
    loop_.Stop();
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
        switch (c) {
            case 'h':
                printf("Unfortunately, there is no any help.");
                ret = -1;
                break;
            case 'c':
                //set conf file to server_config
                conf_file_ = optarg;
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
        log_warn("Set fd(%d) non-block fail. %s", fd, strerror(errno));
        return FAIL;
    }

    return SUCCESS;
}

int TServer::SetCliOpt(int fd) {
    //一般实际缓冲区大小是设置的2倍
    //path:/proc/sys/net/core/w(r)mem_max
    int send_buff_size = 4 * 32768;
    int recv_buff_size = 4 * 32768;

    if (::setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
                     (void *) &send_buff_size, sizeof(send_buff_size)) < 0) {
        log_warn("setsockopt to send buff size fail. %s", strerror(errno));
        return FAIL;
    }

    if (::setsockopt(fd, SOL_SOCKET, SO_RCVBUF,
                     (void *) &recv_buff_size, sizeof(recv_buff_size)) < 0) {
        log_warn("setsockopt to recv buff size fail. %s", strerror(errno));
        return FAIL;
    }

    int nodelay = 1;
    if (::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                     (void *) &nodelay, sizeof(nodelay))) {
        log_warn("setsockopt to forbid Nagle's algorithm. %s", strerror(errno));
        return FAIL;
    }

    return SUCCESS;
}

TServer::ConnectorPtr TServer::FindConn(unsigned long cid) {
    conn_map_t::iterator itr = conn_map_.find(cid);
    if (itr == conn_map_.end()) {
        return NULL;
    }

    return itr->second;
}

int TServer::CheckMask(int mask) {
    if (mask & EV_RDHUP) {
        log_debug("Connect already closed by foreign.");
        return FAIL;
    }

    if (mask & EV_HUP) {
        log_warn("Connect close cause hup.");
        return FAIL;
    }

    if (mask & EV_ERR) {
        log_err("Connect close cause happen err.");
        return FAIL;
    }

    return SUCCESS;
}

TServer::ConnectorPtr TServer::CreateConn(int fd) {
    log_debug("Create Connector for fd %d.", fd);

    ConnectorPtr cli = new Connector(loop_, fd);
    if (NULL == cli) {
        log_err("New a Connector fail.");
        return NULL;
    }

    unsigned long cid = cli->GetCID();
    if (!conn_map_.insert(std::make_pair(cid, cli)).second) {
        log_err("Add cli connector to map fail, fd|%d", fd);
        if (cli)
            delete cli;
        return NULL;
    }

    return cli;
}

void TServer::DelConn(unsigned long cid) {
    log_debug("Connect cid %lu will be closed and del.", cid);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return;
    }

    DelConn(conn);
}

void TServer::DelConn(ConnectorPtr conn) {
    if (conn) {
        conn->Close();
        conn_map_.erase(conn->GetCID());
        delete conn;
    }
}

TServer::TimerTaskPtr TServer::CreateTimerTask(unsigned long cid) {
    log_debug("Create timer task for cid %lu.", cid);

    int timeout = svr_cfg::get_const_instance().timeout;
    TimerTaskPtr timer = new TimerTask(loop_, timeout, 0);
    if (NULL == timer
        || !timer_map_.insert(make_pair(cid, timer)).second) {
        log_warn("Create timer fail for cid %lu.", cid);
        if (timer)
            delete timer;
        return NULL;
    }

    task_data_t data = {.data = {.id = cid}};
    timer->SetPrivateData(data);
    timer->Bind(std::bind(&TServer::OnTimerOut, this, _1, _2, _3));
    timer->Start();

    return timer;
}

void TServer::DelTimerTask(unsigned long cid) {
    log_debug("DelTimerTask cid %lu", cid);

    TimerTaskPtr timer = FindTimer(cid);
    if (!timer) {
        log_warn("Not find timer task for cid %lu.", cid);
        return;
    }

    timer->Stop();
    timer_map_.erase(cid);
    delete timer;
}

TServer::TimerTaskPtr TServer::FindTimer(unsigned long cid) {
    timer_map_t::iterator itr = timer_map_.find(cid);
    if (itr == timer_map_.end()) {
        return NULL;
    }

    return itr->second;
}
