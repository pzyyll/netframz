//
// @Create by Caizhili on 2017/10/10
// @Brief
//

#include <string>
#include <cassert>

#include "zone_connector.h"
#include "log_def.h"
#include "zone_session.h"
#include "zone.h"

using namespace std;

ZoneConnector::ZoneConnector(EventService &es, int fd)
    : Connector(es, fd), session_(NULL) {

}

ZoneConnector::~ZoneConnector() {

}

void ZoneConnector::HandleInput(ErrCode &err_code) {
    if (err_code.get_ret() != ErrCode::SUCCESS) {
        LogWarn("%s", err_code.get_err_msg().c_str());
        Close();
        //TODO release the conn
        return;
    }

    SetLastActTimeToNow();

    while (GetRecvBuff().UsedSize() > 0) {
        LogInfo("Parse cid|%lu", GetCID());
        Cmd *p_cmd = new Cmd;
        assert(p_cmd != NULL);

        char *buff = GetRecvBuff().FrontPos();
        unsigned long len = GetRecvBuff().UsedSize();
        int nr = p_cmd->Parse(buff, len);
        if (nr < 0) {
            LogWarn("Parse cli %lu msg fail. %s", GetCID(), p_cmd->GetErr().c_str());
            Close();
            //TODO
            return;
        }

        if (nr == 0) {
            LogInfo("%s", p_cmd->GetErr().c_str());
            break;
        }

        GetRecvBuff().FrontAdvancing(nr);

        ProcessCmd(p_cmd);
    }
}

int ZoneConnector::ProcessCmd(Cmd *cmd) {
    LogInfo("CmdType|%u|cid|%lu", (unsigned)cmd->GetType(), GetCID());

    switch (cmd->GetType()) {
        case MsgCmd::LOGIN_REQ : {
            if (session_) {
                LogWarn("Repeat login req, session handler is exist.");
                return -1;
            }

            session_ = new ZoneSession(GetCID(), this);
            assert(session_ != NULL);

            ZoneS.QueueSession(session_);

            session_.QueueCmd(cmd);

            return 0;
        }
        default: {
            if (session_ == NULL) {
                LogWarn("Cli not auth session.");
                return -1;
            }
            session_.QueueCmd(cmd);
            return 0;
        }
    }

    return 0;
}

int ZoneConnector::SendCmd(Cmd *cmd) {
    string sndstr;
    cmd->SerializeTo(sndstr);

    this->Send(sndstr.c_str(), sndstr.size());
}
