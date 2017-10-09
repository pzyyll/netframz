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

    int listen_fd = acceptor_.Bind(ipv4, port);
    if (listen_fd < 0) {
        LogErr("Acceptor bind addr fail. %s", acceptor_.GetErrMsg());
        return FAIL;
    }

    if (acceptor_.Listen(std::bind(&Server::OnAccept, this, _1, _2)) < 0) {
        LogErr("Acceptor Listen fail. %s", acceptor_.GetErrMsg());
        return FAIL;
    }

    LogInfo("Start Listen.");
    return listen_fd;
}

int Server::StartTick() {
    int tick_interval =  svr_cfg::get_const_instance().tick;
    tick_ = new TimerTask(es_, tick_interval, 1);
    if (NULL == tick_) {
        LogErr("New fail.");
        return FAIL;
    }
    tick_->Bind(std::bind(&Server::OnTick, this, _1, _2, _3));
    task_data_t pridata = {.data = {.ptr = tick_}};
    tick_->SetPrivateData(pridata);

    tick_->Start();

    LogInfo("Tick start.");
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
    LogInfo("Do fd|%d", fd);

    /*
    if (MakeNonblock(fd) < 0) {
        return;
    }

    if (SetCliOpt(fd) < 0) {
        return;
    }
    */

    ConnectorPtr conn = CreateConn(fd);
    if (!conn) {
        ::close(fd);
        return;
    }

    TimerTaskPtr timer = timer_mgr_.AddTimerTask(
                                        es_,
                                        conn->GetCID(),
                                        svr_cfg::get_const_instance().timeout);//CreateTimerTask(conn->GetCID());
    if (!timer) {
        DelConn(conn);
        return;
    }

    struct ConnCbData cb_data;
    cb_data.pri_data.data.id = conn->GetCID();
    cb_data.handler = std::bind(&Server::OnRead, this, _1, _2, _3);
    conn->Open(cb_data);

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
        LogWarn("Can't find conn for cid %lu in map.", cid);
        return;
    }

    //Parse recv buf
    Parse(*conn);
}

void Server::Parse(Connector &conn) {
    conn.SetLastActTimeToNow();
    while (conn.GetRecvBuff().UsedSize() > 0) {
        LogInfo("Parse cid|%lu", conn.GetCID());
        Cmd cmd;
        char *buff = conn.GetRecvBuff().FrontPos();
        unsigned long len = conn.GetRecvBuff().UsedSize();
        int nr = cmd.Parse(buff, len);
        if (nr < 0) {
            LogWarn("Parse cli %lu msg fail. %s", conn.GetCID(), cmd.GetErr().c_str());
            CloseConn(conn.GetCID());
            return;
        }
        if (nr == 0) {
            LogInfo("%s", cmd.GetErr().c_str());
            break;
        }

        conn.GetRecvBuff().FrontAdvancing(nr);
        //LogInfo("cli data size: %u", (unsigned)cmd.GetMsgData().size());

        ProcessCmd(cmd, conn.GetCID());
    }
}

void Server::ProcessCmd(Cmd &cmd, const unsigned long cid) {
    LogInfo("ProcessCmd");

    //Echo Test
    Response(cmd, cid);
}

void Server::Tick(unsigned long now) {
    UNUSE(now);
    //LogInfo("Tick: %lu", now);
    //Do tick task;
}

int Server::Response(proto::Cmd &cmd, unsigned long cid) {
    std::string sndstr;
    cmd.SerializeTo(sndstr);

    return Response(sndstr.c_str(), sndstr.size(), cid);
}

int Server::Response(const char *buff, unsigned long size, unsigned long cid) {
    LogInfo("Response cid: %lu, size: %lu", cid, size);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        LogWarn("Can't find conn for cid %lu in map.", cid);
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
        LogWarn("cid|%lu, err|%s", cid, err.get_err_msg().c_str());
        //CloseConn(cid);
        return;
    }
}

void Server::OnTick(EventService *es, task_data_t data, int mask) {
    //LogInfo("OnTick");
    UNUSE(es);
    UNUSE(data);
    UNUSE(mask);

    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned long now_ms = now.tv_sec * 1000 + now.tv_usec / 1000;
    Tick(now_ms);
}

void Server::OnTimerOut(EventService *es, task_data_t data, int mask) {
    LogInfo("OnTimerOut.");
    UNUSE(es);
    UNUSE(mask);

    unsigned long cid = data.data.id;

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        LogWarn("Connector for cid %lu not find.", cid);
        timer_mgr_.DelTimerTask(cid);
        return;
    }

    if (conn->IsTimeOut(svr_cfg::get_const_instance().timeout / 1000)) {
        LogInfo("Close cid %lu this long time not act.", cid);
        CloseConn(cid);
        return;
    }

    LogInfo("Resume timer.");
    TimerTaskPtr timer = timer_mgr_.FindTimerTask(cid);
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

int Server::SetMaxFds(int max_fds) {
    LogInfo("SetMaxFds max_fds|%d", max_fds);

    if (max_fds < 0) {
        LogErr("SetMaxFds num invaild|%d", max_fds);
        return FAIL;
    }

    struct rlimit maxfdrl;
    maxfdrl.rlim_cur = max_fds;
    maxfdrl.rlim_max = max_fds;

    if (setrlimit(RLIMIT_NOFILE, &maxfdrl) < 0) {
        LogErr("set rlimit err|%s", strerror(errno));
        return FAIL;
    }

    if (getrlimit(RLIMIT_NOFILE, &maxfdrl) < 0) {
        LogErr("get rlimit err|%s", strerror(errno));
        return FAIL;
    }

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
        LogInfo("Connect already closed by foreign.");
        return FAIL;
    }

    if (mask & EV_HUP) {
        LogWarn("Connect close cause hup.");
        return FAIL;
    }

    if (mask & EV_ERR) {
        LogErr("Connect close cause happen err.");
        return FAIL;
    }

    return SUCCESS;
}

Server::ConnectorPtr Server::CreateConn(int fd) {
    LogInfo("Create Connector for fd %d.", fd);

    ConnectorPtr cli = new Connector(es_, fd);
    if (NULL == cli) {
        LogErr("New a Connector fail.");
        return NULL;
    }

    cli->GetSocket().SetNonBlock(true);
    cli->GetSocket().SetNoDelay(true);
    cli->GetSocket().SetRecvBuff(2 * 65536);
    cli->GetSocket().SetSendBuff(2 * 65536);

    unsigned long cid = cli->GetCID();
    if (!conn_map_.insert(std::make_pair(cid, cli)).second) {
        LogErr("Add cli connector to map fail, fd|%d", fd);
        if (cli)
            delete cli;
        return NULL;
    }

    LogWarn("Debug: Connectsize|%lu", conn_map_.size());
    return cli;
}

void Server::DelConn(unsigned long cid) {
    LogInfo("Connect cid %lu will be closed and del.", cid);

    ConnectorPtr conn = FindConn(cid);
    if (!conn) {
        LogWarn("Can't find conn for cid %lu in map.", cid);
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

bool Server::CheckConnect(unsigned long cid) {
    if (FindConn(cid))
        return true;
    return false;
}

void Server::CloseConn(unsigned long cid) {
    DelConn(cid);
    timer_mgr_.DelTimerTask(cid);
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
