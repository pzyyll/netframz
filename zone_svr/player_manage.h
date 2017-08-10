//
// @Create by CaiZhili on 20170809
// @Brief
//
#ifndef NF_ZONESVR_PLAYERMANAGE_H
#define NF_ZONESVR_PLAYERMANAGE_H

#include <map>
#include <string>

#include "player.h"

class PlayerManage {
public:
    typedef std::map<std::string, Player*> PlayerMap;
    typedef PlayerMap::iterator            PlayerMapItr;
    typedef PlayerMap::const_iterator      PlayerMapConstItr;
public:
    PlayerManage();

    ~PlayerManage();

    Player *AddPlayer(const std::string &name, const unsigned long cid);

    Player *GetPlayer(const std::string &name);

    void DelPlayer(const std::string &name);

    PlayerMapConstItr begin() { return player_map_.begin(); }

    PlayerMapConstItr end() { return player_map_.end(); }

private:
    PlayerMap player_map_;
};

#endif //NF_ZONESVR_PLAYERMANAGE_H
