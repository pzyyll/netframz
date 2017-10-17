//
// @Create by Caizhili on 2017/10/09
// @Brief
//

#include "zone_connect_mgr.h"
#include "log_def.h"

//TODO

ZoneConnectMgr::ZoneConnectMgr() {

}

ZoneConnectMgr::~ZoneConnectMgr() {
    ConnMap::iterator itr;
    for (itr = conn_map_.begin(); itr != conn_map_.end(); ++itr) {
        if (itr->second) {
            itr->second->Close();
            delete itr->second;
        }
    }
}

Connector *ZoneConnectMgr::AddConnector(EventService *es_, int fd) {
    LogInfo("Add Connector for fd %d.", fd);

    Connector *cli = new Connector(*es_, fd);

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

    return cli;
}

Connector *ZoneConnectMgr::FindConn(unsigned long cid) {
    ConnMap::iterator itr = conn_map_.find(cid);
    if (itr != conn_map_.end() && itr->second) {
        return itr->second;
    } else {
        return NULL;
    }
}

bool ZoneConnectMgr::RemoveConn(unsigned long cid) {
    ConnMap::iterator itr = conn_map_.find(cid);
    if (itr != conn_map_.end() && itr->second) {
        delete itr->second;
        conn_map_.erase(itr);
    }

    return false;
}
