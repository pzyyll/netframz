//
// @Create by CaiZhili on 20170808
// @Brief zonesvr.h
//
#ifndef NF_ZONESVR_H
#define NF_ZONESVR_H

#include "server.h"
#include "player_manage.h"
#include "proto/zonesvr.pb.h"
#include "aoi_manage.h"

class ZoneSvr : public Server {
public:
    ZoneSvr();
    ~ZoneSvr();

    virtual int Init(int argc, char **argv);

protected:
    virtual void ProcessCmd(proto::Cmd &cmd, const unsigned long cid) /*override*/;

    void ProcessLogin(const std::string &buff, const unsigned long cid);

    void ProcessLogout(const std::string &buff, const unsigned long cid);

    void ProcessPositionSyn(const std::string &buff, const unsigned long cid);

    void ProcessChat(const std::string &buff, const unsigned long cid);

    void GetOnlinePlayers(std::vector<Player *> &vec_players);

    void SynPlayerPos(Player &player, const std::vector<Player *> &vec_players);

    void SynPlayerLeave(Player &player, const std::vector<Player *> &players);

    void FillZoneStat(ZoneStat &stat, const std::vector<Player *> &vec_players);

    void SendToClient(const ::google::protobuf::Message &msg,
                      const unsigned int type,
                      const unsigned long cid);

    int Parse(::google::protobuf::Message &msg, const std::string &buff);

private:
    AOIManage aoi_manage_;
};

#endif //NF_ZONESVR_H
