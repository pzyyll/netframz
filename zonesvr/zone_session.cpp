//
// @Create by Caizhili on 20170929
// @Brief
//

#include "zone_session.h"

ZoneSession::ZoneSession(unsigned long id, ZoneConnector *conn)
    : id_(id), conn_(conn), player_(NULL) {

}

ZoneSession::~ZoneSession() {

}

void ZoneSession::QueueCmd(Cmd *cmd) {
    cmd_queue_.Push(cmd);
}

bool ZoneSession::Update() {
    Cmd *cmd = NULL;

    while (!conn_->IsClose() && cmd_queue_.Next(cmd)) {
        switch (cmd->GetType()) {
            case MsgCmd::LOGIN_REQ:
                ProcessLogin(*cmd);
                break;
            case MsgCmd::LOGOUT_REQ:
                ProcessLogout(*cmd);
                break;
            case MsgCmd::ZONE_SYN_REQ:
                ProcessPositionSyn(*cmd);
                break;
            case MsgCmd::CHAT_REQ:
                ProcessChat(*cmd);
                break;
            default:
                LogWarn("Cmd Type Err. Type(%u)", (unsigned)cmd->GetType());
                break;
        }
        delete cmd;
        cmd = NULL;
    }

    if (!conn_->IsClose())
        return false;

    return true;
}

void ZoneSession::ProcessLogin(Cmd &cmd) {

}

unsigned long ZoneSession::Get_id() const {
    return id_;
}
