//
// @Create by Caizhili on 2017/10/09
// @Brief
//

#ifndef NF_SVR_ZONE_CONNECT_MGR_H
#define NF_SVR_ZONE_CONNECT_MGR_H

#include <unordered_map>
#include "connector.h"
#include "nf_event.h"
#include "singleton.h"

class ZoneConnectMgr {
public:
    typedef std::unordered_map<unsigned long, Connector *> ConnMap;

    ZoneConnectMgr();

    ~ZoneConnectMgr();

    Connector *AddConnector(EventService *es_, int fd);

    Connector *FindConn(unsigned long cid);

    bool RemoveConn(unsigned long cid);

private:
    ConnMap conn_map_;
};

#define nf::singleton<ZoneConnectMgr>::get_mutable_instance() ZoneConnectMgrS

#endif //NF_SVR_ZONE_CONNECT_MGR_H
