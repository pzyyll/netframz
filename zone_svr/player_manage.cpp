//
// @Create by CaiZhili on 20170809
// Brief
//

#include "player_manage.h"

PlayerManage::PlayerManage() {

}

PlayerManage::~PlayerManage() {

}

Player *Player::AddPlayer(const std::string name, const unsigned int cid) {
    Player *player = NULL;
    if ( (player = GetPlayer(name)) != NULL) {
        return NULL;
    }

    player = new Player;
    if (NULL == player) {
        reutrn NULL;
    }

    player->set_name(name.c_str());
    player->set_conn_id(cid);

    return player;
}

Player *Player::GetPlayer(const std::string name) {
    PlayerMapItr itr = player_map_.find(name);
    if (itr != player_map_.end())
        return itr->second;
    return NULL;
}

void Player::DelPlayer(const std::string name) {
    PlayerMapItr itr = player_map_.find(name);
    if (itr != player_map_.end()) {
        delete itr->second;
        player_map_.erase(itr);
    }
}
