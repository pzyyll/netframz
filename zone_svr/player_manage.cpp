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
        delete itr->second;
        player_map_.erase(itr);
    }
}
