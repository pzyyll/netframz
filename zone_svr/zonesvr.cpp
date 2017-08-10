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
    switch (cmd.GetType()) {
        case MsgCmd::LOGIN_REQ:
            ProcessLogin(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::ZONE_SYN_REQ:
            ProcessPositionSyn(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::CHAT_REQ:
//            ProcessChat(cmd.GetMsgData(), cid);
            break;
        default:
            log_warn("Cmd Type Err. Type(%u)", (unsigned)cmd.GetType());
            break;
    }
}

void ZoneSvr::ProcessLogin(const std::string &buff, const unsigned long cid) {
    log_debug("ProcessLogin. cid|%lu", cid);

    LoginReq req;
    LoginRsp rsp;

    int ret = MsgRet::SUCCESS;

    do {
        if ( (ret = Parse(req, buff)) != MsgRet::SUCCESS) {
            rsp.set_err_msg("Parse req msg err.");
            break;
        }

        //Init player
        Player *player = player_mng_.AddPlayer(req.name(), cid);
        if (!player) {
            ret = MsgRet::FAIL;
            rsp.set_err_msg("Player already exist or unkonw.");
            break;
        }

        ScanAndSynPlayerPosition(*rsp.mutable_zone_stat(), *player);

    } while (false);

    rsp.set_ret(ret);
    SendToClient(rsp, MsgCmd::LOGIN_RSP, cid);
}

void ZoneSvr::ProcessPositionSyn(const std::string &buff, const unsigned long cid) {
    log_debug("ProcessPositionSyn. cid|%lu", cid);

    ZoneSynReq req;
    ZoneSynRsp rsp;

    int ret = MsgRet::SUCCESS;

    do {
        if ( (ret = Parse(req, buff)) != MsgRet::SUCCESS) {
            rsp.set_err_msg("Parse req fail.");
            break;
        }

        Player *player = player_mng_.GetPlayer(req.persion().name());
        if (!player) {
            ret = MsgRet::FAIL;
            rsp.set_err_msg("player is not exist.");
            break;
        }

        int nx, ny, ox, oy;
        ox = player->last_point().x;
        oy = player->last_point().y;
        nx = req.persion().point().x();
        ny = req.persion().point().y();
        if (ox == nx && oy == ny) {
            log_debug("position not change.");
            break;
        }

        player->set_last_point(player->point());
        player->set_point(Point(nx, ny));

        ScanAndSynPlayerPosition(*rsp.mutable_zone_stat(), *player);

    } while (false);

    rsp.set_ret(ret);
    SendToClient(rsp, MsgCmd::ZONE_SYN_RSP, cid);
}

void ZoneSvr::ScanAndSynPlayerPosition(ZoneStat &stat, Player &player) {
    log_debug("ScanAndSynPlayerPosition");

    //Prepare the ZoneSyn of login player send to every online player
    ZoneSyn syn;
    Persion *syn_persion = syn.mutable_zone_stat()->add_persion_list();

    assert(syn_persion == NULL);

    syn_persion->set_name(player.name());
    syn_persion->mutable_point()->set_x(player.point().x);
    syn_persion->mutable_point()->set_y(player.point().y);

    //Scan all online player and add position info of these player to player rsp
    PlayerManage::PlayerMapConstItr itr = player_mng_.begin();
    for ( ; itr != player_mng_.end(); ++itr) {
        Player *pry_player = itr->second;
        if (pry_player->name() != player.name()) {
            Persion *persion = stat.add_persion_list();

            assert(persion == NULL);

            persion->set_name(pry_player->name());
            persion->mutable_point()->set_x(pry_player->point().x);
            persion->mutable_point()->set_y(pry_player->point().y);

            SendToClient(syn, MsgCmd::ZONE_SYN, pry_player->conn_id());
        }
    }
}

void ZoneSvr::SendToClient(const ::google::protobuf::Message &msg,
                           const unsigned int type,
                           const unsigned long cid) {
    log_debug("SendToClient|%s", msg.ShortDebugString().c_str());

    std::string msg_data;
    msg.SerializeToString(&msg_data);

    proto::Cmd cmd;
    cmd.SetType(type);
    cmd.SetMsgData(msg_data);

    Response(cmd, cid);
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
