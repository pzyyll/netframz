//
// @Create by CaiZhili on 20170809
// @Brief
//
#ifndef NF_ZONESVR_PLAYERMANAGE_H
#define NF_ZONESVR_PLAYERMANAGE_H

#include <map>
#include <string>
#include <vector>

#include "player.h"
#include "singleton.h"

class PlayerManage {
public:
    typedef std::map<std::string, Player*> PlayerMap;
    typedef PlayerMap::iterator            PlayerMapItr;
    typedef PlayerMap::const_iterator      PlayerMapConstItr;
    typedef std::map<unsigned long, std::string> IdMap;
    typedef IdMap::iterator                      IdMapItr;

public:
    PlayerManage();

    ~PlayerManage();

    Player *AddPlayer(const std::string &name, const unsigned long cid);

    Player *GetPlayer(const std::string &name);

    void DelPlayer(const std::string &name);

    Player *GetPlayerByCid(const unsigned long cid);

    void GetPlayersByCid(std::vector<Player *> &players,
                         const std::vector<unsigned long> &cids);

    void DelPlayerByCid(const unsigned long cid);

    PlayerMapConstItr begin() { return player_map_.begin(); }

    PlayerMapConstItr end() { return player_map_.end(); }

private:
    PlayerMap player_map_;
    IdMap cid_map_;
};

#define PlayerMgrS nf::singleton<PlayerManage>::get_mutable_instance()

#endif //NF_ZONESVR_PLAYERMANAGE_H
