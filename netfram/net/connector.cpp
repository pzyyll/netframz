//
// @ Create by CaiZhili on 2017/7/18
// @ Brief
//

#include "connector.h"
#include "mem_check.h"

#define UNUSE(x) (void)(x);

using namespace std::placeholders;
using namespace nf;

unsigned long Connector::id_cnt_ = 1;

Connector::Connector(EventService &es, int fd)
        : cid_(GetIdCnt()),
          is_close_(0),
          last_act_time_(0),
          recv_buf_(DEFAULT_BUFF_SIZE),
          send_buf_(DEFAULT_BUFF_SIZE),
          task_(new IOTask(es, fd, 0)),
          socket_(fd) {
    SetLastActTimeToNow();
}

Connector::~Connector() {
    Close();
    if (task_)
        delete task_;
    task_ = NULL;
}

void Connector::BeginRecv(const ConnCbData &cb_data) {
    rdata_ = cb_data;

    task_->SetMask(EV_READABLE);
    task_->Bind(std::bind(&Connector::OnRead, this, _1, _2, _3));
    task_->SetReadHandle(std::bind(&Connector::OnRead, this, _1, _2, _3));
    task_->SetWriteHandle(std::bind(&Connector::OnWriteRemain, this, _1, _2, _3));
    task_->Start();
}

void Connector::OnRead(EventService *es, task_data_t data, int mask) {
    ErrCode err_code(ErrCode::SUCCESS);
    ssize_t nr = 0;
    UNUSE(es);
    UNUSE(data);

    do {
        if (CheckMask(mask) < 0) {
            err_code.set_ret(ErrCode::FAIL);
            err_code.set_err_msg(err_msg_);
            nr = 0;
            break;
        }

        //检查用户缓冲区是否满了
        recv_buf_.MemoryMove2Left();
        ssize_t remain = recv_buf_.RemainSize();
        if (remain == 0) {
            nr = 0;
            break;
        }

        char *base = recv_buf_.TailPos();
        nr = socket_.Recv(base, remain); //InnerRead(base, remain);
        if (nr < 0) {
            snprintf(err_msg_, sizeof(err_msg_), "%s", socket_.GetErrMsg());
            err_code.set_ret(ErrCode::FAIL);
            err_code.set_err_msg(err_msg_);
            nr = 0;
        }
    } while (false);

    recv_buf_.TailAdvancing(nr);

    if (rdata_.handler)
        rdata_.handler(recv_buf_.UsedSize(), rdata_.pri_data, err_code);
}

void Connector::OnWriteRemain(EventService *es, task_data_t data, int mask) {
    ssize_t ns = 0;
    UNUSE(es);
    UNUSE(data);

    if (CheckMask(mask) < 0) {
        Close();
        return;
    }

    ns = SendRemain();
    if (ns < 0) {
        Close();
        return;
    }

    //cancel writebale
    if (send_buf_.UsedSize() == 0) {
        task_->RemoveMask(EV_WRITEABLE);
        task_->Restart();
        return;
    }
}

int Connector::CheckMask(int mask) {
    if (mask & EV_RDHUP) {
        snprintf(err_msg_, sizeof(err_msg_), "Connect already closed by foreign.");
        return FAIL;
    }

    if (mask & EV_HUP) {
        snprintf(err_msg_, sizeof(err_msg_), "Connect close cause hup.");
        return FAIL;
    }

    if (mask & EV_ERR) {
        snprintf(err_msg_, sizeof(err_msg_), "Connect close cause happen err.");
        return FAIL;
    }

    return SUCCESS;
}

Connector::size_t Connector::Recv(void *buff, const size_t size) {
    size_t remain = recv_buf_.UsedSize();
    size_t take_size = size;
    char *base = recv_buf_.FrontPos();

    if (take_size > remain) {
        take_size = remain;
    }

    memcpy(buff, base, take_size);
    recv_buf_.FrontAdvancing(take_size);

    return take_size;
}

Connector::ssize_t Connector::Send(const char *buff,
                                   const size_t lenth,
                                   const ConnCbData &cb_data) {
    ErrCode err_code(ErrCode::SUCCESS);

    send_buf_.MemoryMove2Left();
    // Check buff remain size
    if (send_buf_.RemainSize() < lenth) {
        snprintf(err_msg_, sizeof(err_msg_), "send buff full.");
        return -1;
    }

    memcpy(send_buf_.TailPos(), (char *) buff, lenth);
    send_buf_.TailAdvancing(lenth);

    //schedule send
    task_->AddMask(EVSTAT::EV_WRITEABLE);
    task_->Restart();
}

Connector::ssize_t Connector::SendRemain() {
    if (send_buf_.UsedSize() == 0) {
        return 0;
    }

    void *buff = send_buf_.FrontPos();
    size_t lenth = send_buf_.UsedSize();
    ssize_t nw = socket_.Send(buff, lenth);
    if (nw < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", socket_.GetErrMsg());
        return FAIL;
    }
    send_buf_.FrontAdvancing(nw);

    return nw;
}

void Connector::Close() {
    if (is_close_)
        return;

    //To make close operate
    task_->Stop();
    socket_.Close();

    is_close_ = true;
}

bool Connector::IsTimeOut(const unsigned long time_limit) {
    struct timeval now;
    gettimeofday(&now, NULL);
    if ((unsigned long) now.tv_sec - last_act_time_ >= time_limit) {
        return true;
    }

    return false;
}

void Connector::SetLastActTimeToNow() {
    struct timeval now;
    gettimeofday(&now, NULL);
    last_act_time_ = now.tv_sec;
}

struct Buffer &Connector::GetRecvBuff() {
    return recv_buf_;
}

unsigned long Connector::GetCID() {
    return cid_;
}

std::string Connector::GetErrMsg() {
    return std::string(err_msg_);
}

unsigned long Connector::GetIdCnt() {
    return id_cnt_++;
}
