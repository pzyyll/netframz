//
// @Create by CaiZhili on 20170808
// @Brief zonesvr.h
//
#ifndef NF_ZONESVR_H
#define NF_ZONESVR_H

#include "server.h"
#include "player_manage.h"
#include "proto/zonesvr.pb.h"

class ZoneSvr : public BaseServer {
public:
    ZoneSvr();
    ~ZoneSvr();

protected:
    virtual void ProcessCmd(proto::Cmd &cmd, const unsigned long cid) /*override*/;

    void ProcessLogin(const std::string &buff, const unsigned long cid);

    void ProcessPositionSyn(const std::string &buff, const unsigned long cid);

    void ProcessChat(const std::string &buff, const unsigned long cid);

    void SendToClient(const ::google::protobuf::Message &msg,
                      const unsigned int type,
                      const unsigned long cid);

    int Parse(::google::protobuf::Message &msg, const std::string &buff);

private:
    PlayerManage player_mng_;
};

#endif //NF_ZONESVR_H
