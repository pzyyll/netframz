//
// @Create by Caizhili on 20170906
// @Brief
//

#ifndef NF_ZONESVR_ZONE_H
#define NF_ZONESVR_ZONE_H

#include <map>

#include "nf_lock_queue.h"
#include "singleton.h"

class ZoneSession;

typedef std::map<unsigned long, ZoneSession *> MapSession;

class Zone {
public:
    Zone();

    ~Zone();

    //TODO
    int Update();

    void QueueSession(ZoneSession *session);

private:
    //TODO(@Caizhili)

    MapSession map_session_;

    nf::LockQueue<ZoneSession *> queue_session_;
};

#define nf::singleton<Zone>::get_mutable_instance() ZoneS

#endif //NF_ZONESVR_ZONE_H
