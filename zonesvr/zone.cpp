//
// @Create By Caizhili on 20170906
// @Brief
//

#include "zone.h"

Zone::Zone() {

}

Zone::~Zone() {

}

int Zone::Update() {
    //TODO

    MapSession::iterator next, itr;
    for (itr = map_session_.begin(); itr != map_session_.end(); itr = next) {
        next = ++itr;
        //TODO
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
