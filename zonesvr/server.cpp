/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#include "server.h"
#include "server_config.h"
#include "log.h"
#include "mem_check.h"
#include "log_def.h"

#include <iostream>

#define UNUSE(x) ((void)(x))

using namespace std::placeholders;
using namespace std;
using namespace proto;
using namespace nf;

Server::Server() : conf_file_(NULL), tick_(NULL) {

}

Server::~Server() {
    if (tick_)
        delete tick_;
}

int Server::Init(int argc, char **argv) {
    if (GetOption(argc, argv) < 0) {
        return FAIL;
    }

    if (svr_cfg::get_mutable_instance().Init(conf_file_) < 0) {
        return FAIL;
    }

    log_init(argv[0], svr_cfg::get_const_instance().syslog_level);

    InitLog(argv[0],
            svr_cfg::get_const_instance().log_level,
            svr_cfg::get_const_instance().log_path.c_str());

    if (svr_cfg::get_const_instance().daemon &&
        Daemon() == FAIL) {
        return FAIL;
    }

    if (es_.Init() < 0) {
        return FAIL;
    }

    if (SetMaxFds(svr_cfg::get_const_instance().max_fds) < 0) {
        return FAIL;
    }

    if (StartListen() < 0) {
        return FAIL;
    }

    if (StartTick() < 0) {
        return FAIL;
    }

    return SUCCESS;
}

int Server::StartListen() {
    acceptor_.SetEventService(&es_);

    std::string ipv4 = svr_cfg::get_const_instance().ipv4;
    int port = svr_cfg::get_const_instance().port;

    log_info("Bind addr: %s, port: %d", ipv4.c_str(), port);

    int listen_fd = acceptor_.Bind(ipv4, port);
    if (listen_fd < 0) {
        log_err("Acceptor bind addr fail. %s", acceptor_.GetErrMsg());
        return FAIL;
    }

    if (acceptor_.Listen(std::bind(&Server::OnAccept, this, _1, _2)) < 0) {
        log_err("Acceptor Listen fail. %s", acceptor_.GetErrMsg());
        return FAIL;
    }

    log_debug("Start Listen.");
    return listen_fd;
}

int Server::StartTick() {
    int tick_interval =  svr_cfg::get_const_instance().tick;
    tick_ = new TimerTask(es_, tick_interval, 1);
    if (NULL == tick_) {
        log_err("New fail.");
        return FAIL;
    }
    tick_->Bind(std::bind(&Server::OnTick, this, _1, _2, _3));
    task_data_t pridata = {.data = {.ptr = tick_}};
    tick_->SetPrivateData(pridata);

    tick_->Start();

    log_debug("Tick start.");
    return SUCCESS;
}

void Server::OnAccept(int fd, ErrCode &err) {
    if (err.get_ret() != ErrCode::SUCCESS) {
        log_info("%s", err.get_err_msg().c_str());
        return;
    }

    Do(fd);
}

void Server::Do(int fd) {
    log_debug("Do fd|%d", fd);

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

    TimerTaskPtr timer = CreateTimerTask(conn->GetCID());
    if (!timer) {
        DelConn(conn);
        return;
    }

    struct ConnCbData cb_data;
    cb_data.pri_data.data.id = conn->GetCID();
    cb_data.handler = std::bind(&Server::OnRead, this, _1, _2, _3);
    conn->BeginRecv(cb_data);

    task_data_t data = {.data = {.id = conn->GetCID()}};
    timer->SetPrivateData(data);
    timer->Bind(std::bind(&Server::OnTimerOut, this, _1, _2, _3));
    timer->Start();
}

void Server::OnRead(unsigned long lenth, task_data_t data, ErrCode err) {
    UNUSE(lenth);
    unsigned long cid = data.data.id;
    if (err.get_ret() != ErrCode::SUCCESS) {
        log_info("%s", err.get_err_msg().c_str());
        CloseConn(cid);
        return;
    }

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return;
    }

    //Parse recv buf
    Parse(*conn);
}

void Server::Parse(Connector &conn) {
    log_debug("Do");
    conn.SetLastActTimeToNow();
    while (conn.GetRecvBuff().UsedSize() > 0) {
        Cmd cmd;
        char *buff = conn.GetRecvBuff().FrontPos();
        unsigned long len = conn.GetRecvBuff().UsedSize();
        int nr = cmd.Parse(buff, len);
        if (nr < 0) {
            log_warn("Parse cli %lu msg fail. %s", conn.GetCID(), cmd.GetErr().c_str());
            CloseConn(conn.GetCID());
            return;
        }
        if (nr == 0) {
            log_debug("%s", cmd.GetErr().c_str());
            break;
        }

        conn.GetRecvBuff().FrontAdvancing(nr);
        log_debug("cli data size: %u", (unsigned)cmd.GetMsgData().size());

        ProcessCmd(cmd, conn.GetCID());
    }
}

void Server::ProcessCmd(Cmd &cmd, const unsigned long cid) {
    log_debug("ProcessCmd");

    //Echo Test
    Response(cmd, cid);
}

void Server::Tick(unsigned long now) {
    UNUSE(now);
    //log_debug("Tick: %lu", now);
    //Do tick task;
}

int Server::Response(proto::Cmd &cmd, unsigned long cid) {
    std::string sndstr;
    cmd.SerializeTo(sndstr);
    log_debug("snd: %li", (long int)sndstr.size());

    return Response(sndstr.c_str(), sndstr.size(), cid);
}

int Server::Response(const char *buff, unsigned long size, unsigned long cid) {
    log_debug("Response: %lu, size: %lu", cid, size);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return FAIL;
    }

    struct ConnCbData cbdata;
    cbdata.pri_data.data.id = cid;
    cbdata.handler = std::bind(&Server::OnWriteErr, this,_1, _2, _3);
    conn->Send(buff, size, cbdata);

    return SUCCESS;
}

void Server::OnWriteErr(unsigned long lenth, task_data_t data, ErrCode err) {
    UNUSE(lenth);
    unsigned long cid = data.data.id;
    if (err.get_ret() != ErrCode::SUCCESS) {
        log_warn("%s", err.get_err_msg().c_str());
        CloseConn(cid);
        return;
    }
}

void Server::OnTick(EventService *es, task_data_t data, int mask) {
    //log_debug("OnTick");
    UNUSE(es);
    UNUSE(data);
    UNUSE(mask);

    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned long now_ms = now.tv_sec * 1000 + now.tv_usec / 1000;
    Tick(now_ms);
}

void Server::OnTimerOut(EventService *es, task_data_t data, int mask) {
    log_debug("OnTimerOut.");
    UNUSE(es);
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
        CloseConn(cid);
        return;
    }

    log_debug("Resume timer.");
    TimerTaskPtr timer = FindTimer(cid);
    timer->Restart();
}

void Server::Run() {
    es_.Run();
}

void Server::Stop() {
    es_.Stop();
}

//=====================private===========================//
int Server::GetOption(int argc, char **argv) {
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

int Server::MakeNonblock(int fd) {
    int val = 0;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        log_warn("Get fd(%d) stat fail. %s", fd, strerror(errno));
    }

    val |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, val) < 0) {
        log_warn("Set fd(%d) non-block fail. %s", fd, strerror(errno));
        return FAIL;
    }

    return SUCCESS;
}

int Server::SetCliOpt(int fd) {
    //一般实际缓冲区大小是设置的2倍
    //path:/proc/sys/net/core/w(r)mem_max
    int send_buff_size = 2 * 65536;
    int recv_buff_size = 2 * 65536;

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
        log_warn("setsockopt to disabling Nagle's algorithm. %s", strerror(errno));
        return FAIL;
    }

    return SUCCESS;
}

int Server::SetMaxFds(int max_fds) {
    log_debug("SetMaxFds max_fds|%d", max_fds);

    if (max_fds < 0) {
        log_err("SetMaxFds num invaild|%d", max_fds);
        return FAIL;
    }

    struct rlimit maxfdrl;
    maxfdrl.rlim_cur = max_fds;
    maxfdrl.rlim_max = max_fds;

    if (setrlimit(RLIMIT_NOFILE, &maxfdrl) < 0) {
        log_err("set rlimit err|%s", strerror(errno));
        return FAIL;
    }

    if (getrlimit(RLIMIT_NOFILE, &maxfdrl) < 0) {
        log_err("get rlimit err|%s", strerror(errno));
        return FAIL;
    }
    log_info("The maximum fds|%d", (int)maxfdrl.rlim_cur);

    return SUCCESS;
}

Server::ConnectorPtr Server::FindConn(unsigned long cid) {
    conn_map_t::iterator itr = conn_map_.find(cid);
    if (itr == conn_map_.end()) {
        return NULL;
    }

    return itr->second;
}

int Server::CheckMask(int mask) {
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

Server::ConnectorPtr Server::CreateConn(int fd) {
    log_debug("Create Connector for fd %d.", fd);

    ConnectorPtr cli = new Connector(es_, fd);
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

void Server::DelConn(unsigned long cid) {
    log_debug("Connect cid %lu will be closed and del.", cid);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return;
    }

    DelConn(conn);
}

void Server::DelConn(ConnectorPtr conn) {
    if (conn) {
        conn->Close();
        conn_map_.erase(conn->GetCID());
        delete conn;
    }
}

Server::TimerTaskPtr Server::CreateTimerTask(unsigned long cid) {
    log_debug("Create timer task for cid %lu.", cid);

    int timeout = svr_cfg::get_const_instance().timeout;
    TimerTaskPtr timer = new TimerTask(es_, timeout, 0);
    if (NULL == timer
        || !timer_map_.insert(make_pair(cid, timer)).second) {
        log_warn("Create timer fail for cid %lu.", cid);
        if (timer)
            delete timer;
        return NULL;
    }

    return timer;
}

void Server::DelTimerTask(unsigned long cid) {
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

Server::TimerTaskPtr Server::FindTimer(unsigned long cid) {
    timer_map_t::iterator itr = timer_map_.find(cid);
    if (itr == timer_map_.end()) {
        return NULL;
    }

    return itr->second;
}

bool Server::CheckConnect(unsigned long cid) {
    if (FindConn(cid))
        return true;
    return false;
}

void Server::CloseConn(unsigned long cid) {
    DelConn(cid);
    DelTimerTask(cid);
}

int Server::Daemon() {
    //Linux 有现成的 daemon() 调用可以将进程转为守护进程

    pid_t pid;

    if ( (pid = fork()) < 0)
        return FAIL;
    else if (pid)
        _exit(0);

    if (setsid() < 0)
        return FAIL;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        return FAIL;

    if ( (pid = fork()) < 0)
        return FAIL;
    else if (pid)
        _exit(0);

    umask(0);

    const int kMaxFd = 64;
    for (int i = 0; i < kMaxFd; ++i)
        close(i);

    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    return SUCCESS;
}
