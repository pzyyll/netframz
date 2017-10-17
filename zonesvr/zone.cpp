//
// @Create By Caizhili on 20170906
// @Brief
//

#include "zone.h"
#include "zone_session.h"

Zone::Zone() {

}

Zone::~Zone() {

}

int Zone::Update() {
    //TODO

    UpdateSession();

    CmdList *cmd_broad;
    while (queue_broad_.Next(cmd_broad)) {
        std::vector<unsigned long> &ids = cmd_broad->second;
        Cmd *cmd = cmd_broad->first;

        for (unsigned i = 0; i < ids.size(); ++i) {
            ZoneSession *sess = FindSession(ids[i]);
            if (sess)
                sess->SendCmd(*cmd);
        }
    }

    return 0;
}

void Zone::QueueSession(ZoneSession *session) {
    queue_session_.Push(session);
}

void Zone::MsgBroadcast(const std::vector<unsigned long> &ids, Cmd *cmd) {
    CmdList *cmdl = new CmdList;

    cmdl->first = cmd;
    cmdl->second = ids;

    queue_broad_.Push(cmdl);
}

ZoneSession *Zone::FindSession(unsigned long id) {
    MapSession::iterator itr = map_session_.find(id);
    if (itr != map_session_.end()) {
        return itr->second;
    } else {
        return NULL;
    }
}

int Zone::RemoveSession(unsigned long id) {
    MapSession::iterator itr = map_session_.find(id);
    if (itr != map_session_.end() && itr->second) {
        itr->second->Kick();
        return 0;
    }

    return -1;
}

void Zone::AddSession(ZoneSession *session) {
    assert(session != NULL);

    map_session_[session->Get_id()] = session;
}

void Zone::UpdateSession() {
    ZoneSession *sess;
    while (queue_session_.Next(sess)) {
        AddSession(sess);
    }

    MapSession::iterator next, itr;
    for (itr = map_session_.begin(); itr != map_session_.end(); itr = next) {
        next = itr;
        ++next;
        //TODO
        ZoneSession *p_session = itr->second;
        if (!p_session->Update()) {
            map_session_.erase(itr);
            delete p_session;
        }
    }
}
