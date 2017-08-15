//
// @Create by CaiZhili on 20170809
// Brief
//

#include "player_manage.h"

PlayerManage::PlayerManage() {

}

PlayerManage::~PlayerManage() {

}

Player *PlayerManage::AddPlayer(const std::string &name, const unsigned long cid) {
    Player *player = NULL;
    if ( (player = GetPlayer(name)) != NULL) {
        return NULL;
    }

    player = new Player;
    if (NULL == player) {
        return NULL;
    }

    player->set_name(name.c_str());
    player->set_conn_id(cid);

    player_map_.insert(std::make_pair(name, player));
    cid_map_.insert(std::make_pair(cid, name));

    return player;
}

Player *PlayerManage::GetPlayer(const std::string &name) {
    PlayerMapItr itr = player_map_.find(name);
    if (itr != player_map_.end())
        return itr->second;
    return NULL;
}

void PlayerManage::DelPlayer(const std::string &name) {
    PlayerMapItr itr = player_map_.find(name);
    if (itr != player_map_.end()) {
        cid_map_.erase(itr->second->conn_id());

        delete itr->second;
        player_map_.erase(itr);
    }
}

Player *PlayerManage::GetPlayerByCid(const unsigned long cid) {
    IdMapItr finditr = cid_map_.find(cid);
    if (finditr != cid_map_.end())
        return GetPlayer(finditr->second);
    return NULL;
}

void PlayerManage::DelPlayerByCid(const unsigned long cid) {
    IdMapItr finditr = cid_map_.find(cid);
    if (finditr != cid_map_.end())
        DelPlayer(finditr->second);
}
