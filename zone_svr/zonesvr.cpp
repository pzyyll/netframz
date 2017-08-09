//
// @Create by Caizhili on 20170808
// @Brief zonesvr.cpp
//

#include <cassert>
#include "zonesvr.h"
#include "log.h"

using namespace proto;

ZoneSvr::ZoneSvr() {

}

ZoneSvr::~ZoneSvr() {

}

void ZoneSvr::ProcessCmd(proto::Cmd &cmd, const unsigned long cid) {
    log_debug("ProcessCmd");
    switch (cmd->GetType()) {
        case MsgCmd::LOGINREQ:
            ProcessLogin(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::ZONESYN:
            ProcessPositionSyn(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::CHATREQ:
            ProcessChat(cmd.GetMsgData(), cid);
            break;
        default:
            break;
    }
}

void ZoneSvr::ProcessLogin(const std::string &buff, const unsigned long cid) {
    LoginReq req;
    LoginRsp rsp;

    //Parse login req from string buff
    if (Parse(req, buff) < 0) {
        rsp.set_ret(MsgRet::FAIL);
        rsp.set_err_msg("Parse req msg err.")
        SendToClient(rsp, MsgCmd::LOGINRSP, cid);
        return;
    }

    //Init player
    Player *player = player_mng_.AddPlayer(req.name(), cid);
    if (!player) {
        rsp.set_ret(MsgRet::FAIL);
        rsp.set_err_msg("Player already exist or unkonw.");
        SendToClient(rsp, MsgCmd::LOGINRSP, cid);
        return;
    }

    //Prepare the ZoneStat of login player send to every online player
    ZoneStat syn_stat;
    Persion *login_persion = syn_stat.add_persion_list();

    assert(login_persion == NULL);

    login_persion->set_name(player->name());
    login_persion->mutable_point()->set_x(player->x());
    login_persion->mutable_point()->set_y(player->y());

    //Scan all online player and add position info of these player to login player rsp
    PlayerManage::PlayerMapConstItr itr = player_mng_.begin();
    for ( ; itr != player_mng_.end(); ++itr) {
        Player *pry_player = itr->second();
        if (pry_player->name() != player->name()) {
            Persion *persion = rsp.add_persion_list();

            assert(persion == NULL);

            persion->set_name(pry_player->name());
            persion->mutable_point()->set_x(pri_player->x());
            persion->mutable_point()->set_y(pry_player->y());

            SendToClient(syn_stat, MsgCmd::ZONESTAT, pry_player->conn_id());
        }
    }

    rsp.set_ret(MsgRet::SUCCESS);
    SendToClient(rsp, MsgCmd::LOGINRSP, cid);
}

void SendToClient(const ::google::protobuf::Message &msg,
                  const unsigned int type,
                  const unsigned long cid) {
    log_debug("SendToClient|%s", msg.ShortDebugString().c_str());
    std::string msg_data;
    msg.SerializeToString(msg_data);

    proto::Cmd cmd;
    cmd.SetType(type);
    cmd.SetMsgData(msg_data);

    Response(cid, cmd);
}

int ZoneSvr::Parse(::google::protobuf::Message &msg, const std::string &buff) {
    log_debug("Buff|%u", (unsigned)buff.size());
    if (!msg.ParseFromString(buff)) {
        log_warn("Decode msg fail|%s", msg.GetTypeName().c_str());
        return MsgRet::FAIL;
    }

    log_debug("Parse|%s", msg.ShortDebugString().c_str());
    return MsgRet::SUCCESS;
}
