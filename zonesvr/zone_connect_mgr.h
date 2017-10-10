//
// @Create by Caizhili on 2017/10/09
// @Brief
//

#ifndef NF_SVR_ZONE_CONNECT_MGR_H
#define NF_SVR_ZONE_CONNECT_MGR_H

#include <unordered_map>
#include "connector.h"
#include "nf_event.h"

class ZoneConnectMgr {
public:
    typedef Connector *ConnectorPtr;
    typedef std::unordered_map<unsigned long, ConnectorPtr> ConnMap;

    ZoneConnectMgr();

    ~ZoneConnectMgr();

    ConnectorPtr AddConnector(EventService *es_, int fd);

    //TODO
    ConnectorPtr FindConn(unsigned long cid);


private:
    ConnMap conn_map_;
};

#endif //NF_SVR_ZONE_CONNECT_MGR_H
