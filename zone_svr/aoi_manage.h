//
// @Created by czllo on 2017/8/17.
// @Brief
//

#ifndef NF_ZONESVR_AOI_MANAGE_H
#define NF_ZONESVR_AOI_MANAGE_H

#include <algorithm>
#include <list>
#include <vector>

struct Vec2 {
    Vec2(int xt = 0, int yt = 0) : x(xt), y(yt) { }
    ~Vec2() { }

    //提供 set 求交集，差集的比较
    bool operator < (const Vec2 &a) const {
        if (x < a.x)
            return true;
        if (x > a.x)
            return false;
        if (y < a.y)
            return true;
        return false;
    }

    int x;
    int y;
};

struct Pos {
    Pos(int xt = 0, int yt = 0) : x(xt), y(yt) { }
    ~Pos() { }

    int x;
    int y;
};

class Object {
    unsigned long id;
    Pos last_pos;
    Pos pos;
};

class AOIEntry {
    std::list<Object *> obj_list;
};

class AOIManage {
public:
    const static unsigned int kMaxX = 10;
    const static unsigned int kMaxY = 10;

    typedef std::map<unsigned long, Object *> Id2ObjMap;
public:
    AOIManage();

    ~AOIManage();

    void AddPos(unsigned long id, const Pos &pos);

    void DelPos(unsigned long id, std::vector<unsigned long> &remove_ids);

    void UpdataPos(unsigned long id, const Pos &pos, std::vector<unsigned long> &remove_ids, std::vector<unsigned long> &interest_ids);

    //todo

private:
    AOIEntry vec2map[kMaxX][kMaxY];
    Id2ObjMap id2obj_map;
};

#endif //NF_ZONESVR_AOI_H
