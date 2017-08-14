//
// @Create by CaiZhili on 20170814
// @Brief
//
#include "client_api.h"

int Connect(Client *cli) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(cli->conn_port);
    if (inet_pton(AF_INET, cli->conn_ip, &addr.sin_addr) < 0)
        return -1;

    if (connect(cli->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -1;

    return 0;
}

int Writen(int fd, const void *buff, const unsigned int lenth) {
    int          nw = 0;
    unsigned int nleft = lenth;
    const char   *cptr = (const char *)buff;

    while (nleft > 0) {
        nw = write(fd, cptr, nleft);
        if (nw <= 0) {
            if (EINTR == errno)
                continue;
            if (EAGAIN == errno)
                break;
            return -1;
        }

        nleft -= nw;
        cptr += nw;
    }

    return (lenth - nleft);
}

int Readn(int fd, void *buff, const unsigned int lenth) {
    int          rw = 0;
    unsigned int nleft = lenth;
    char         *cptr = (char *)buff;

    while (nleft > 0) {
        rw = read(fd, cptr, nleft);
        if (rw < 0) {
            if (EINTR == errno)
                continue;
            if (EAGAIN == errno)
                break;
            return -1;
        } elsef if (rw == 0) {
            break;
        }

        nleft -= rw;
        cptr += rw;
    }

    return (lenth - nleft);
}

int MakeBlockStatus(int fd, bool block) {
    int val = fcntl(fd, F_GETFL, 0);
    if (val < 0)
        return -1;

    if (block) {
        val &= ~O_NONBLOCK;
    } else {
        val |= O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, val) < 0)
        return -1;

    return 0;
}

/////////////////////////////////////////////////////////
#define MAX_SHOW_MSGS 10

static int sock_fd;
static char rv_buff[1024 * 1024];
static unsigned int rv_len;
static int login_wait;

Persion persion;
std::map<std::string, Persion> persions_map;
std::deque<std::string> chat_msgs;

int InputOption() {
    //todo
    int ret = 0;
    std::string option;

    std::cin >> option;
    if (option.size() != 1) {
        cout << "Option invalid." << endl;
        return ret;
    }

    struct Pos pos = { 0, 0 };
    switch (option[0]) {
        case 'h':
            pos.x = -1;
            Move(pos);
            break;
        case 'i':
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
            ret = -1;
            break;
        default:
            break;
    }

    return ret;
}

void CliRun(Client *cli) {
    sock_fd = cli->fd;

    if (Connect(cli) < 0) {
        std::cout << "Connect fail." << std::endl;
        return;
    }

    pthread_t tid;
    pthread_create(&tid, NULL, RecvHandler, NULL);

    //todo client input
    Login();

    while (true) {
        //todo input
        if (InputOption() < 0)
            break;
    }
}

void *Recvhandler(void *args) {
    int nr = 0;
    while(true) {
        nr = Readn(sock_fd, rv_buff + rv_len, sizeof(rv_buff) - rv_len);
        if (nr < 0) {
            std::cout << "read err" << std::endl;
            break;
        } else if (nr == 0) {
            std::cout << "close by peer." << std::endl;
            break;
        }

        rv_len += nr;
        //todo process buff
        int np = ProcessBuff(rv_buff, rv_len);

        if (np < 0) {
            std::cout << "parse err" << std::endl;
            break;
        }

        rv_len -= np;
        memmove(rv_buff, rv_buff + np, rv_len);
    }

    pthread_exit(NULL);
}

int ProcessBuff(const char *buff, const unsigned int lenth) {
    unsigned int upos = 0;
    while (upos < lenth) {
        proto::Cmd cmd;
        int np = cmd.Parse(buff + upos, lenth - upos);
        if (np < 0)
            return -1;

        if (np == 0)
            break;

        ProcessRspCmd(cmd);

        upos += np;
    }

    return np;
}

int ProcessCmd(proto::Cmd &cmd) {
    switch (cmd.GetType()) {
        case MsgCmd::LOGIN_RSP:
            ProcessLoginRsp(cmd.GetMsgData());
            break;
        case MsgCmd::ZONE_SYN_RSP:
            ProcessZoneSynRsp(cmd.GetMsgData());
            break;
        case MsgCmd::ZONE_SYN:
            ProcessZoneSyn(cmd.GetMsgData());
            break;
        case MsgCmd::CHAT_RSP:
            ProcessChatRsp(cmd.GetMsgData());
            break;
        case MsgCmd::CHAT_STAT:
            ProcessChatStat(cmd.GetMsgDatq());
            break;
        default:
            break;
    }
}

int ProcessLoginRsp(const std::string &data) {
    LoginRsp rsp;
    if (!rsp.ParseFromString(data)) {
        return -1;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        cout << rsp.err_msg() << endl;
        return -1;
    }

    for (int i = 0; i < rsp.zone_stat().persion_list_size(); ++i) {
        const Persion &persion = rsp.zone_stat().persion_list(i);
        persions_map[persion.name()] = persion;
    }

    FreshShow();
    login_wait = 1;

    return 0;
}

int ProcessZoneSynRsp(const std::string &data) {
    ZoneSynRsp rsp;
    if (!rsp.ParseFromString(data)) {
        return -1;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        std::cout << rsp.err_msg() << std::endl;
        return -1;
    }

    for (int i = 0; i < rsp.zone_stat().persion_list_size(); ++i) {
        const Persion &persion = rsp.zone_stat().persion_list(i);
        persions_map[persion.name()] = persion;
    }

    FreshShow();

    return 0;
}

int ProcessZoneSyn(const std::string &data) {
    ZoneSyn syn;

    if (!syn.ParseFromString(data)) {
        return -1;
    }

    for (int i = 0; i < syn.zone_stat().persion_list_size(); ++i) {
        const Persion &persion = rsp.zone_stat().persion_list(i);
        persions_map[persion.name()] = persion;
    }

    FreshShow();

    return 0;
}

int ProcessChatRsp(const std::string &data) {
    ChatRsp rsp;

    if (!rsp.ParseFromString(data)) {
        return -1;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        std::cout << rsp.err_msg() << std::endl;
        return -1;
    }
}

int ProcessChatStat(const std::string &data) {
    ChatStat cs;

    if (!cs.ParseFromString(data)) {
        return -1;
    }

    std::string speaker_name = cs.speaker().name();
    time_t speaker_time = cs.time();
    std::string content = cs.speaker().content();

    std::string time_str(ctime(&speaker_time));

    std::string show_str = time_str + " " + speaker_name + " " + content;
    chat_msgs.push_back(show_str);

    FreshShow();
}

void FreshShow() {
    std::cout << "Pos: " << std::endl;
    auto itr = persions_map.begin();
    for ( ; itr != persion_map.end(); ++itr) {
        Persion &persion = itr.second;
        std::cout << persion.name() << "("
                  << persion.point().x() << ","
                  << persion.point().y() << ")"
                  << std::endl;
    }

    std::cout << "Chat:" << std::endl;
    auto citr = chat_msgs.begin();
    for ( ; citr != chat_msgs.end(); ++citr) {
        std::cout << *citr << std::endl;
    }
}

int Login() {
    std::cout << "login name: ";
    std::string name;
    std::cin >> name;

    LoginReq req;
    req.set_name(name);

    login_wait = 0;
    SendMsgToSvr(req, MsgCmd::LOGIN_REQ);

    std::cout << "loging..." << std::endl;
    while (!login_wait) usleep(100);
}

int SendMsgToSvr(::google::protobuf::Message &msg, unsigned int type) {
    std::string str;
    msg.SerializeToString(&str);

    proto::Cmd cmd;
    cmd.SetType(type);
    cmd.SetMsgData(str);

    str.clear();
    cmd.Serialize(str);
    //std::cout << "snd: " << str.size() << endl;
    int ns = Writen(sock_fd, (void *)str.c_str(), str.size());
    if (ns < 0) {
        std::cout << "Send err." << std::endl;
        return -1;
    }

    return 0;
}
