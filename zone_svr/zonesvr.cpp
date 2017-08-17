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
            ProcessChat(cmd.GetMsgData(), cid);
            break;
        default:
            log_warn("Cmd Type Err. Type(%u)", (unsigned)cmd.GetType());
            break;
    }
}

void ZoneSvr::CloseConn(unsigned long cid) {
    BaseServer::CloseConn(cid);

    //Send Leave info to other online users
    ZoneUserRemove leave_info;
    Player *player = player_mng_.GetPlayerByCid(cid);
    if (player) {
        leave_info.set_name(player->name());

        std::vector<Player *> vec_players;
        GetOnlinePlayers(vec_players);

        for (unsigned int i = 0; i < vec_players.size(); ++i) {
            SendToClient(leave_info,
                         MsgCmd::ZONE_USER_REMOVE,
                         vec_players[i]->conn_id());
        }

        player_mng_.DelPlayerByCid(cid);
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

        std::vector<Player *> vec_players;

        GetOnlinePlayers(vec_players);

        SynPlayerPos(*player, vec_players);

        FillZoneStat(*rsp.mutable_zone_stat(), vec_players);

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


//	        int nx, ny, ox, oy;
//	        ox = player->last_point().x;
//	        oy = player->last_point().y;
//	        nx = req.persion().point().x();
//	        ny = req.persion().point().y();
//	        if (ox == nx && oy == ny) {
//	            log_debug("position not change.");
//	            break;
//	        }

        int nx, ny;
        nx = req.persion().point().x();
        ny = req.persion().point().y();

        player->set_last_point(player->point());
        player->set_point(Point(nx, ny));

        std::vector<Player *> vec_players;

        GetOnlinePlayers(vec_players);

        SynPlayerPos(*player, vec_players);
    } while (false);

    rsp.set_ret(ret);
    SendToClient(rsp, MsgCmd::ZONE_SYN_RSP, cid);
}

void ZoneSvr::ProcessChat(const std::string &buff, const unsigned long cid) {
    log_debug("ProcessChat");

    ChatReq req;
    ChatRsp rsp;

    int ret = MsgRet::SUCCESS;

    do {
        if ( (ret = Parse(req, buff)) != MsgRet::SUCCESS) {
            rsp.set_err_msg("Parse req fail.");
            break;
        }

        Player *player = player_mng_.GetPlayer(req.name());
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
    PlayerManage::PlayerMapConstItr itr = player_mng_.begin();
    for ( ; itr != player_mng_.end(); ++itr) {
        Player *pry_player = itr->second;
        assert(pry_player != NULL);

        if (CheckConnect(pry_player->conn_id())) {
            vec_players.push_back(pry_player);
        } else {
            //todo detach offline player, but not here.
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
    log_debug("SendToClient|%s|cid %lu|", msg.ShortDebugString().c_str(), cid);

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
