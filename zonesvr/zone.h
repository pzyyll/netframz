//
// @Create by Caizhili on 20170906
// @Brief
//

#ifndef NF_ZONESVR_ZONE_H
#define NF_ZONESVR_ZONE_H

#include <map>
#include <vector>
#include <utility>

#include "nf_lock_queue.h"
#include "singleton.h"

class ZoneSession;

typedef std::map<unsigned long, ZoneSession *> MapSession;
typedef std::pair<Cmd *cmd, std::vector<unsigned long>> CmdList;

class Zone {
public:
    Zone();

    ~Zone();

    //TODO
    int Update();

    void QueueSession(ZoneSession *session);

    void MsgBroadcast(const std::vector<unsigned long> &ids, Cmd *cmd);

    ZoneSession *FindSession(unsigned long id);

    bool RemoveSession(unsigned long id);

private:
    void AddSession(ZoneSession *session);

    void UpdateSession();

private:
    //TODO(@Caizhili)

    MapSession map_session_;

    nf::LockQueue<ZoneSession *> queue_session_;

    nf::LockQueue<CmdList *> queue_broad_;
};

#define nf::singleton<Zone>::get_mutable_instance() ZoneS

#endif //NF_ZONESVR_ZONE_H
