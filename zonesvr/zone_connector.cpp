//
// @Create by Caizhili on 2017/10/10
// @Brief
//

#include "zone_connector.h"
#include "log_def.h"
#include "zone_session.h"

#include <cassert>

ZoneConnector::ZoneConnector(EventService &es, int fd)
    : Connector(es, fd), session_(NULL) {

}

ZoneConnector::~ZoneConnector() {

}

//TODO

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

    if (cmd->GetType() == MsgCmd::LOGIN_REQ) {

    }
}


