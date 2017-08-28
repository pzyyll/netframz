/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#include "server.h"
#include "server_config.h"
#include "log.h"
#include "mem_check.h"

#define UNUSE(x) ((void)(x))

using namespace std::placeholders;
using namespace std;
using namespace proto;
using namespace nf;

BaseServer::BaseServer() : conf_file_(NULL), accept_task_(NULL), tick_(NULL) {

}

BaseServer::~BaseServer() {
    if (accept_task_)
        delete accept_task_;
    if (tick_)
        delete tick_;
}

int BaseServer::Init(int argc, char **argv) {
    if (GetOption(argc, argv) < 0) {
        return FAIL;
    }

    if (svr_cfg::get_mutable_instance().Init(conf_file_) < 0) {
        return FAIL;
    }

    log_init(argv[0], svr_cfg::get_const_instance().log_level);

    if (svr_cfg::get_const_instance().daemon &&
        Daemon() == FAIL) {
        return FAIL;
    }

    if (loop_.Init() < 0) {
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

int BaseServer::StartListen() {
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
    accept_task_->Bind(std::bind(&BaseServer::OnAccept, this, _1, _2, _3));
    task_data_t data = {.data = {.ptr = accept_task_}};
    accept_task_->SetPrivateData(data);

    if (accept_task_->Start() < 0) {
        log_err("%s", accept_task_->GetErr().c_str());
        return FAIL;
    }

    log_debug("Listen start. fd|%d", listen_fd);
    return listen_fd;
}

int BaseServer::StartTick() {
    int tick_interval =  svr_cfg::get_const_instance().tick;
    tick_ = new TimerTask(loop_, tick_interval, 1);
    if (NULL == tick_) {
        log_err("New fail.");
        return FAIL;
    }
    tick_->Bind(std::bind(&BaseServer::OnTick, this, _1, _2, _3));
    task_data_t pridata = {.data = {.ptr = tick_}};
    tick_->SetPrivateData(pridata);

    tick_->Start();

    log_debug("Tick start.");
    return SUCCESS;
}

void BaseServer::OnAccept(EventLoop *loopsv, task_data_t data, int mask) {
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

        Do(fd);
    }
}

void BaseServer::Do(int fd) {
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
    cb_data.handler = std::bind(&BaseServer::OnRead, this, _1, _2, _3);
    conn->BeginRecv(cb_data);

    task_data_t data = {.data = {.id = conn->GetCID()}};
    timer->SetPrivateData(data);
    timer->Bind(std::bind(&BaseServer::OnTimerOut, this, _1, _2, _3));
    timer->Start();
}

void BaseServer::OnRead(unsigned long lenth, task_data_t data, ErrCode err) {
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

void BaseServer::Parse(Connector &conn) {
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

void BaseServer::ProcessCmd(Cmd &cmd, const unsigned long cid) {
    log_debug("ProcessCmd");

    //Echo Test
    Response(cmd, cid);
}

void BaseServer::Tick(unsigned long now) {
    UNUSE(now);
    //log_debug("Tick: %lu", now);
    //Do tick task;
}

int BaseServer::Response(proto::Cmd &cmd, unsigned long cid) {
    std::string sndstr;
    cmd.Serialize(sndstr);
    log_debug("snd: %li", (long int)sndstr.size());

    return Response(sndstr.c_str(), sndstr.size(), cid);
}

int BaseServer::Response(const char *buff, unsigned long size, unsigned long cid) {
    log_debug("Response: %lu, size: %lu", cid, size);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return FAIL;
    }

    struct ConnCbData cbdata;
    cbdata.pri_data.data.id = cid;
    cbdata.handler = std::bind(&BaseServer::OnWriteErr, this,_1, _2, _3);
    conn->Send(buff, size, cbdata);

    return SUCCESS;
}

void BaseServer::OnWriteErr(unsigned long lenth, task_data_t data, ErrCode err) {
    UNUSE(lenth);
    unsigned long cid = data.data.id;
    if (err.get_ret() != ErrCode::SUCCESS) {
        log_warn("%s", err.get_err_msg().c_str());
        CloseConn(cid);
        return;
    }
}

void BaseServer::OnTick(EventLoop *loopsv, task_data_t data, int mask) {
    //log_debug("OnTick");
    UNUSE(loopsv);
    UNUSE(data);
    UNUSE(mask);

    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned long now_ms = now.tv_sec * 1000 + now.tv_usec / 1000;
    Tick(now_ms);
}

void BaseServer::OnTimerOut(EventLoop *loopsv, task_data_t data, int mask) {
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
        CloseConn(cid);
        return;
    }

    log_debug("Resume timer.");
    TimerTaskPtr timer = FindTimer(cid);
    timer->Restart();
}

void BaseServer::Run() {
    loop_.Run();
}

void BaseServer::Stop() {
    loop_.Stop();
}

//=====================private===========================//
int BaseServer::GetOption(int argc, char **argv) {
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

int BaseServer::MakeNonblock(int fd) {
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

int BaseServer::SetCliOpt(int fd) {
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

int BaseServer::SetMaxFds(int max_fds) {
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

BaseServer::ConnectorPtr BaseServer::FindConn(unsigned long cid) {
    conn_map_t::iterator itr = conn_map_.find(cid);
    if (itr == conn_map_.end()) {
        return NULL;
    }

    return itr->second;
}

int BaseServer::CheckMask(int mask) {
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

BaseServer::ConnectorPtr BaseServer::CreateConn(int fd) {
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

void BaseServer::DelConn(unsigned long cid) {
    log_debug("Connect cid %lu will be closed and del.", cid);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        log_warn("Can't find conn for cid %lu in map.", cid);
        return;
    }

    DelConn(conn);
}

void BaseServer::DelConn(ConnectorPtr conn) {
    if (conn) {
        conn->Close();
        conn_map_.erase(conn->GetCID());
        delete conn;
    }
}

BaseServer::TimerTaskPtr BaseServer::CreateTimerTask(unsigned long cid) {
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

    return timer;
}

void BaseServer::DelTimerTask(unsigned long cid) {
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

BaseServer::TimerTaskPtr BaseServer::FindTimer(unsigned long cid) {
    timer_map_t::iterator itr = timer_map_.find(cid);
    if (itr == timer_map_.end()) {
        return NULL;
    }

    return itr->second;
}

bool BaseServer::CheckConnect(unsigned long cid) {
    if (FindConn(cid))
        return true;
    return false;
}

void BaseServer::CloseConn(unsigned long cid) {
    DelConn(cid);
    DelTimerTask(cid);
}

int BaseServer::Daemon() {
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
