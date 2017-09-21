
#include "client_worker.h"

using namespace std::placeholders;
using namespace std;
using namespace nf;

ClientWorker::ClientWorker()
    : rv_len_(0),
      login_wait_(0),
      rcv_task_(NULL),
      is_close_(true) {

}

ClientWorker::~ClientWorker() {
    Logout();

    if (rcv_task_) {
        delete rcv_task_;
        rcv_task_ = NULL;
    }
}

void ClientWorker::Start(const Endpoint &ep, const std::string &name) {
    //todo
    name_ = name;
    ep_ = ep;

    snd_.SetHandle(std::bind(&ClientWorker::SndHandler, this, _1));
    snd_.Run(NULL);
}

void ClientWorker::Wait() {
    snd_.Join();
    rcv_.Join();
}

int ClientWorker::SendMsgToSvr(::google::protobuf::Message &msg, unsigned int type) {
    std::string str;
    msg.SerializeToString(&str);

    proto::Cmd cmd;
    cmd.SetType(type);
    cmd.SetMsgData(str);

    str.clear();
    cmd.SerializeTo(str);

    int ns = socket_.Send((void *)str.c_str(), str.size());
    if (ns < 0) {
        std::cout << "Send err." << std::endl;
        return -1;
    }

    return 0;
}

int ClientWorker::CheckMask(int mask) {
    if (mask & EV_RDHUP) {
        return -1;
    }

    if (mask & EV_HUP) {
        return -1;
    }

    if (mask & EV_ERR) {
        return -1;
    }

    return 0;
}

void ClientWorker::RcvCb(EventService *es, task_data_t data, int mask) {
    if (CheckMask(mask) < 0) {
        std::cout << "close" << std::endl;
        rcv_task_->Stop();
        return;
    }

    int nr = socket_.Recv(rv_buff_ + rv_len_, sizeof(rv_buff_) - rv_len_);
    if (nr < 0) {
        std::cout << "Recv err" << std::endl;
        return;
    } else if (nr == 0) {
        std::cout << "close by peer." << std::endl;
        return;
    }

    rv_len_ += nr;

    int np = ParseBuff(rv_buff_, rv_len_);
    if (np < 0) {
        std::cout << "parse err" << std::endl;
        rcv_task_->Stop();
        return;
    }

    rv_len_ -= np;
    memmove(rv_buff_, rv_buff_ + np, rv_len_);
}

void ClientWorker::RcvHandler(void *args) {
    (void)(args);
    std::cout << "Start Rcv" << std::endl;

    socket_.SetNonBlock(true);
    es_.Init();
    rcv_task_ = new IOTask(es_, socket_.GetSock(), EVSTAT::EV_READABLE,
                           std::bind(&ClientWorker::RcvCb, this, _1, _2, _3));
    rcv_task_->Start();

    es_.Run();

    pthread_exit(NULL);
}

void ClientWorker::SndHandler(void *args) {
    std::cout << ep_.GetIP() << endl;
    if (socket_.Connect(ep_, 5000)) {
        std::cout << "Connect fail. " << socket_.GetErrMsg() << std::endl;
        return;
    }

    is_close_ = 0;

    rcv_.SetHandle(std::bind(&ClientWorker::RcvHandler, this, _1));
    rcv_.Run(NULL);

    Login(name_);

    while (true) {
        if (InputOption() < 0) break;
        sleep(1);
    }
    es_.Stop();
}

int ClientWorker::ParseBuff(const char *buff, const unsigned int lenth) {
    unsigned int upos = 0;
    while (upos < lenth) {
        proto::Cmd cmd;
        int np = cmd.Parse(buff + upos, lenth - upos);
        if (np < 0) return -1;
        if (np == 0) break;

        ProcessCmd(cmd);
        upos += np;
    }
    return (int)upos;
}

int ClientWorker::ProcessCmd(proto::Cmd &cmd) {
    int ret = 0;
    switch (cmd.GetType()) {
        case MsgCmd::LOGIN_RSP:
            ret = ProcessLoginRsp(cmd.GetMsgData());
            break;
        case MsgCmd::ZONE_SYN_RSP:
            ret = ProcessZoneSynRsp(cmd.GetMsgData());
            break;
        case MsgCmd::ZONE_SYN:
            ret = ProcessZoneSyn(cmd.GetMsgData());
            break;
        case MsgCmd::ZONE_USER_REMOVE:
            ret = ProcessZoneUserRemove(cmd.GetMsgData());
            break;
        case MsgCmd::CHAT_RSP:
            ret = ProcessChatRsp(cmd.GetMsgData());
            break;
        case MsgCmd::CHAT_STAT:
            ret = ProcessChatStat(cmd.GetMsgData());
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

int ClientWorker::ProcessLoginRsp(const std::string &data) {
    LoginRsp rsp;
    if (!rsp.ParseFromString(data)) {
        return -1;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        std::cout << rsp.err_msg() << std::endl;
        return -1;
    }

    //for (int i = 0; i < rsp.zone_stat().persion_list_size(); ++i) {
    //    const Persion &persion = rsp.zone_stat().persion_list(i);
    //    persions_map[persion.name()] = persion;
    //}

    login_wait_ = 1;

    return 0;
}

int ClientWorker::ProcessZoneSynRsp(const std::string &data) {
    ZoneSynRsp rsp;
    if (!rsp.ParseFromString(data)) {
        return -1;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        std::cout << rsp.err_msg() << std::endl;
        return -1;
    }

    //std::cout << rsp.ShortDebugString() << std::endl;

    //persions_map.clear();
    //for (int i = 0; i < rsp.show_stat().persion_list_size(); ++i) {
    //    const Persion &persion = rsp.show_stat().persion_list(i);
    //    persions_map[persion.name()] = persion;
    //}

    //for (int i = 0; i < rsp.unshow_stat().persion_list_size(); ++i) {
    //    const Persion &persion = rsp.unshow_stat().persion_list(i);
    //    persions_map.erase(persion.name());
    //}

    //FreshShow();

    return 0;
}

int ClientWorker::ProcessZoneSyn(const std::string &data) {
    ZoneSyn syn;

    if (!syn.ParseFromString(data)) {
        return -1;
    }

    //std::cout << syn.ShortDebugString() << std::endl;



    //for (int i = 0; i < syn.zone_stat().persion_list_size(); ++i) {
    //    const Persion &persion = syn.zone_stat().persion_list(i);
    //    persions_map[persion.name()] = persion;
    //}

    return 0;
}

int ClientWorker::ProcessZoneUserRemove(const std::string &data) {
    ZoneUserRemove logout_info;

    if (!logout_info.ParseFromString(data)) {
        return -1;
    }

    //todo
    return 0;
}

int ClientWorker::ProcessChatRsp(const std::string &data) {
    ChatRsp rsp;

    if (!rsp.ParseFromString(data)) {
        return -1;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        std::cout << rsp.err_msg() << std::endl;
        return -1;
    }

    //std::cout << rsp.ShortDebugString() << std::endl;


    return 0;
}

int ClientWorker::ProcessChatStat(const std::string &data) {
    ChatStat cs;

    if (!cs.ParseFromString(data)) {
        return -1;
    }

    //std::cout << cs.ShortDebugString() << std::endl;


    //std::string speaker_name = cs.speaker().name();
    //time_t speaker_time = cs.time();
    //std::string content = cs.content();

    //std::string time_str(ctime(&speaker_time));

    //std::string show_str = time_str + speaker_name + ":  " + content;
    //chat_msgs.push_back(show_str);

    return 0;
}

int ClientWorker::Login(const std::string &name) {
    self_info_.set_name(name);
    self_info_.mutable_point()->set_x(0);
    self_info_.mutable_point()->set_y(0);

    LoginReq req;
    req.set_name(name);

    login_wait_ = 0;
    if (SendMsgToSvr(req, MsgCmd::LOGIN_REQ) < 0)
        return -1;

    std::cout << "loging..." << std::endl;
    while (!login_wait_) usleep(100);

    return 0;
}

void ClientWorker::Logout() {
    if (is_close_)
        return;

    LogoutReq req;
    req.set_name(self_info_.name());

    SendMsgToSvr(req, MsgCmd::LOGOUT_REQ);
}

int ClientWorker::Move(Pos mv_direct) {
    int x = self_info_.point().x();
    int y = self_info_.point().y();

    x += mv_direct.x; y += mv_direct.y;

    if (x < 0) x = 0;
    if (x >= MAX_COL) x = MAX_COL - 1;
    if (y < 0) y = 0;
    if (y >= MAX_ROW) y = MAX_ROW - 1;

    self_info_.mutable_point()->set_x(x);
    self_info_.mutable_point()->set_y(y);

    ZoneSynReq req;
    req.mutable_persion()->CopyFrom(self_info_);

    return SendMsgToSvr(req, MsgCmd::ZONE_SYN_REQ);;
}

int ClientWorker::Chat() {
    std::string content = "robot-test";
    ChatReq req;
    req.set_name(self_info_.name());
    req.set_content(content);

    return SendMsgToSvr(req, MsgCmd::CHAT_REQ);
}

void ClientWorker::RandOption(char &option) {
    char opt_v[5] = {'h', 'l', 'j', 'k', 'c'};
    option = opt_v[rand() % 5];
}

int ClientWorker::InputOption() {
    //todo
    int ret = 0;
    char option;

    RandOption(option);

    //std::cin >> option;

    struct Pos pos = { 0, 0 };
    switch (option) {
        case 'h':
            pos.x = -1;
            Move(pos);
            break;
        case 'l':
            pos.x = +1;
            Move(pos);
            break;
        case 'j':
            pos.y = +1;
            Move(pos);
            break;
        case 'k':
            pos.y = -1;
            Move(pos);
            break;
        case 'c':
            Chat();
            break;
        case 'q':
            Logout();
            ret = -1;
            break;
        default:
            break;
    }

    return ret;
}
