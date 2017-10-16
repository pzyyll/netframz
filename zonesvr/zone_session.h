//
// @Create by Caizhili on 20170929
// @Brief
//

#ifndef NF_ZONESVR_ZONE_SESSION_H
#define NF_ZONESVR_ZONE_SESSION_H

#include "proto/zonesvr.pb.h"
#include "proto.h"
#include "nf_lock_queue.h"

class ZoneConnector;

class Player;

class ZoneSession {
public:
    ZoneSession(unsigned long id, ZoneConnector *conn);

    ~ZoneSession();

    void QueueCmd(Cmd *cmd);

    bool Update();

    //TODO
    void ProcessLogin(Cmd &cmd);

    void ProcessLogout(Cmd &cmd);

    void ProcessPositionSyn(Cmd &cmd);

    void ProcessChat(Cmd &cmd);
    //

    unsigned long Get_id() const;

    int SendMsg(const ::google::protobuf::Message &msg,
                const unsigned int type);

    void Kick();

private:
    //TODO
    unsigned long id_;

    ZoneConnector *conn_;

    Player *player_;

    nf::LockQueue<Cmd *> cmd_queue_;
};

#endif
