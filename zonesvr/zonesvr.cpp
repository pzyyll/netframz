//
// @Create by Caizhili on 20170808
// @Brief zonesvr.cpp
//

#include <cassert>
#include "zonesvr.h"
#include "log.h"
#include "log_def.h"

using namespace proto;

ZoneSvr::ZoneSvr() {

}

ZoneSvr::~ZoneSvr() {

}

int ZoneSvr::Init(int argc, char **argv) {
    if (Server::Init(argc, argv) < 0)
        return -1;

    aoi_manage_.Init();

    return 0;
}

void ZoneSvr::ProcessCmd(proto::Cmd &cmd, const unsigned long cid) {
    LogInfo("CmdType|%u|cid|%lu", (unsigned)cmd.GetType(), cid);
    switch (cmd.GetType()) {
        case MsgCmd::LOGIN_REQ:
            ProcessLogin(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::LOGOUT_REQ:
            ProcessLogout(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::ZONE_SYN_REQ:
            ProcessPositionSyn(cmd.GetMsgData(), cid);
            break;
        case MsgCmd::CHAT_REQ:
            ProcessChat(cmd.GetMsgData(), cid);
            break;
        default:
            LogWarn("Cmd Type Err. Type(%u)", (unsigned)cmd.GetType());
            break;
    }
}

void ZoneSvr::CloseConn(unsigned long cid) {
    Server::CloseConn(cid);

    NotifyLeave(cid);
}

void ZoneSvr::NotifyLeave(unsigned long cid) {
    //Send Leave info to other online users
    Player *player = PlayerMgrS.GetPlayerByCid(cid);
    if (player) {
        std::vector<unsigned long> noti_ids;
        aoi_manage_.DelPos(cid, noti_ids);

        std::vector<Player *> noti_players;
        PlayerMgrS.GetPlayersByCid(noti_players, noti_ids);

        SynPlayerLeave(*player, noti_players);

        PlayerMgrS.DelPlayerByCid(cid);
    }
}


void ZoneSvr::ProcessLogin(const std::string &buff, const unsigned long cid) {
    LogInfo("ProcessLogin. cid|%lu", cid);

    LoginReq req;
    LoginRsp rsp;

    int ret = MsgRet::SUCCESS;

    do {
        if ( (ret = Parse(req, buff)) != MsgRet::SUCCESS) {
            rsp.set_err_msg("Parse req msg err.");
            break;
        }

        //Init player
        Player *player = PlayerMgrS.AddPlayer(req.name(), cid);
        if (!player) {
            ret = MsgRet::FAIL;
            rsp.set_err_msg("Player already exist or unkonw.");
            break;
        }

        std::vector<unsigned long> interest_ids;
        aoi_manage_.AddPos(cid,
                           Pos(player->point().x, player->point().y),
                           interest_ids);

        std::vector<Player *> inter_players;
        PlayerMgrS.GetPlayersByCid(inter_players, interest_ids);

        SynPlayerPos(*player, inter_players);

        FillZoneStat(*rsp.mutable_zone_stat(), inter_players);

    } while (false);

    rsp.set_ret(ret);
    SendToClient(rsp, MsgCmd::LOGIN_RSP, cid);
}

void ZoneSvr::ProcessLogout(const std::string &buff, const unsigned long cid) {
    LogInfo("ProcessLogout. cid|%lu", cid);

    LogoutReq req;

    if (Parse(req, buff) != MsgRet::SUCCESS)
        return;

    NotifyLeave(cid);
}

void ZoneSvr::ProcessPositionSyn(const std::string &buff, const unsigned long cid) {
    LogInfo("ProcessPositionSyn. cid|%lu", cid);

    ZoneSynReq req;
    ZoneSynRsp rsp;

    int ret = MsgRet::SUCCESS;

    do {
        if ( (ret = Parse(req, buff)) != MsgRet::SUCCESS) {
            rsp.set_err_msg("Parse req fail.");
            break;
        }

        Player *player = PlayerMgrS.GetPlayer(req.persion().name());
        if (!player) {
            ret = MsgRet::FAIL;
            rsp.set_err_msg("player is not exist.");
            break;
        }

        int nx, ny;
        nx = req.persion().point().x();
        ny = req.persion().point().y();

        player->set_last_point(player->point());
        player->set_point(Point(nx, ny));

        std::vector<unsigned long> rm_ids, inter_ids;
        aoi_manage_.UpdatePos(cid, Pos(nx, ny), rm_ids, inter_ids);

        std::vector<Player *> notify_rms, notify_inters;

        PlayerMgrS.GetPlayersByCid(notify_rms, rm_ids);
        PlayerMgrS.GetPlayersByCid(notify_inters, inter_ids);

        SynPlayerLeave(*player, notify_rms);
        SynPlayerPos(*player, notify_inters);

        FillZoneStat(*rsp.mutable_unshow_stat(), notify_rms);
        FillZoneStat(*rsp.mutable_show_stat(), notify_inters);
    } while (false);

    rsp.set_ret(ret);
    SendToClient(rsp, MsgCmd::ZONE_SYN_RSP, cid);
}

void ZoneSvr::ProcessChat(const std::string &buff, const unsigned long cid) {
    LogInfo("ProcessChat. cid|%lu", cid);

    ChatReq req;
    ChatRsp rsp;

    int ret = MsgRet::SUCCESS;

    do {
        if ( (ret = Parse(req, buff)) != MsgRet::SUCCESS) {
            rsp.set_err_msg("Parse req fail.");
            break;
        }

        Player *player = PlayerMgrS.GetPlayer(req.name());
        if (!player) {
            ret = MsgRet::FAIL;
            rsp.set_err_msg("player is not exist.");
            break;
        }

        ChatStat cs;
        Persion &speaker = *cs.mutable_speaker();
        speaker.set_name(player->name());
        speaker.mutable_point()->set_x(player->point().x);
        speaker.mutable_point()->set_y(player->point().y);
        cs.set_content(req.content());
        cs.set_time(time(NULL));

        std::vector<Player *> vec_players;

        GetOnlinePlayers(vec_players);

        for (unsigned int i = 0; i < vec_players.size(); ++i) {
            SendToClient(cs, MsgCmd::CHAT_STAT, vec_players[i]->conn_id());
        }
    } while (false);

    rsp.set_ret(ret);
    SendToClient(rsp, MsgCmd::CHAT_RSP, cid);
}

void ZoneSvr::GetOnlinePlayers(std::vector<Player *> &vec_players) {
    PlayerManage::PlayerMapConstItr itr = PlayerMgrS.begin();
    for ( ; itr != PlayerMgrS.end(); ++itr) {
        Player *pry_player = itr->second;
        assert(pry_player != NULL);

        if (CheckConnect(pry_player->conn_id())) {
            vec_players.push_back(pry_player);
        } 
    }
}

void ZoneSvr::SynPlayerPos(Player &player, const std::vector<Player *> &vec_players) {
    ZoneSyn syn;
    Persion *syn_persion = syn.mutable_zone_stat()->add_persion_list();

    assert(syn_persion != NULL);

    syn_persion->set_name(player.name());
    syn_persion->mutable_point()->set_x(player.point().x);
    syn_persion->mutable_point()->set_y(player.point().y);

    for (unsigned int i = 0; i < vec_players.size(); ++i) {
        assert(vec_players[i]);
        if (vec_players[i]->name() != player.name())
            SendToClient(syn, MsgCmd::ZONE_SYN, vec_players[i]->conn_id());
    }
}

void ZoneSvr::SynPlayerLeave(Player &player,
                             const std::vector<Player *> &players) {
    ZoneUserRemove leave_info;
    leave_info.set_name(player.name());

    for (unsigned int i = 0; i < players.size(); ++i) {
        SendToClient(leave_info,
                     MsgCmd::ZONE_USER_REMOVE,
                     players[i]->conn_id());
    }
}

void ZoneSvr::FillZoneStat(ZoneStat &stat, const std::vector<Player *> &vec_players) {
    for (unsigned int i = 0; i < vec_players.size(); ++i) {
        Player *player = vec_players[i];
        Persion *persion = stat.add_persion_list();
        assert(persion != NULL);

        persion->set_name(player->name());
        persion->mutable_point()->set_x(player->point().x);
        persion->mutable_point()->set_y(player->point().y);
    }
}

void ZoneSvr::SendToClient(const ::google::protobuf::Message &msg,
                           const unsigned int type,
                           const unsigned long cid) {
    LogInfo("SendToClient|%s|cid|%lu|type|%u", msg.ShortDebugString().c_str(), cid, type);

    proto::Cmd cmd;
    cmd.SetType(type);
    cmd.SetMsgData(msg);

    Response(cmd, cid);
}

int ZoneSvr::Parse(::google::protobuf::Message &msg, const std::string &buff) {
    if (!msg.ParseFromString(buff)) {
        LogWarn("Decode msg fail|%s", msg.GetTypeName().c_str());
        return MsgRet::FAIL;
    }

    LogInfo("Parse|%s", msg.ShortDebugString().c_str());
    return MsgRet::SUCCESS;
}
