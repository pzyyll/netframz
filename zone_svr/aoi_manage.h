//
// @Created by czllo on 2017/8/17.
// @Brief
//

#ifndef NF_ZONESVR_AOI_MANAGE_H
#define NF_ZONESVR_AOI_MANAGE_H

#include <algorithm>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <cmath>

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

struct Object {
    unsigned long id;
    Pos pos;
};

struct AOIEntry {
    std::list<Object *> obj_list;
};

class AOIManage {
public:
    const static unsigned int kMax = 10;
    const static unsigned int kMaxWidth = 20;
    const static unsigned int kMaxHeight = 20;

    typedef std::map<unsigned long, Object *> Id2ObjMap;
public:
    AOIManage();

    ~AOIManage();

    int AddPos(const unsigned long id,
               const Pos &pos,
               std::vector<unsigned long> &interest_ids);

    void DelPos(unsigned long id, std::vector<unsigned long> &remove_ids);

    void UpdatePos(const unsigned long id,
                   const Pos &pos,
                   std::vector<unsigned long> &remove_ids,
                   std::vector<unsigned long> &interest_ids);

private:
    void CalcGridSet(const Vec2 &vec2, std::set<Vec2> &vec2_set);

    bool CheckVec2(const Vec2 &vec2);

    Object *FindObj(const unsigned long id);

    void GetUserIdsFromGrid(const std::set<Vec2> &vec2_set,
                            std::vector<unsigned long> &ids);

    void GetInterestUserIds(const Vec2 &vec2, std::vector<unsigned long> &ids);

private:
    AOIEntry vec2map_[kMax][kMax];
    Id2ObjMap id2obj_map_;
};

#endif //NF_ZONESVR_AOI_H
