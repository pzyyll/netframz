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
#include <string>
#include <cmath>

struct Vec2 {
    Vec2(int xt = 0, int yt = 0) : x(xt), y(yt) { }
    ~Vec2() { }

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

struct Rect {
    Rect(unsigned int width = 0, unsigned int height = 0)
        : w(width), h(height) {  }
    ~Rect() {  }

    unsigned int w;
    unsigned int h;
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
    const static int kDeafultMapWidth = 20;
    const static int kDeafultMapHeight = 20;
    const static int kDeafultGridWidth = 8;
    const static int kDeafultGridHeight = 8;
    const static unsigned int kErrMsgMaxLen = 256;

    typedef std::map<unsigned long, Object *> Id2ObjMap;

    enum Ret {
        FAIL = -1,
        SUCCESS = 0,
    };

public:
    AOIManage();

    ~AOIManage();

    int Init(Rect map_size = Rect(kDeafultMapWidth, kDeafultMapHeight),
             Rect grid_size = Rect(kDeafultGridWidth, kDeafultGridHeight));

    int AddPos(const unsigned long id,
               const Pos &pos,
               std::vector<unsigned long> &interest_ids);

    void DelPos(unsigned long id, std::vector<unsigned long> &remove_ids);

    void UpdatePos(const unsigned long id,
                   const Pos &pos,
                   std::vector<unsigned long> &remove_ids,
                   std::vector<unsigned long> &interest_ids);

    void GetAllUserId(std::vector<unsigned long> &users_id);

    std::string GetErrMsg();

private:
    Vec2 CalcVec2(const Pos &pos);

    unsigned int IndexOf(const Vec2 &vec2);

    void CalcGridSet(const Vec2 &vec2, std::set<Vec2> &vec2_set);

    bool CheckVec2(const Vec2 &vec2);

    Object *FindObj(const unsigned long id);

    void GetUserIdsFromGrid(const std::set<Vec2> &vec2_set,
                            std::vector<unsigned long> &ids);

    void GetInterestUserIds(const Vec2 &vec2, std::vector<unsigned long> &ids);

private:
    AOIEntry *grids_;

    Id2ObjMap id2obj_map_;

    Rect map_size_;

    Rect grid_size_;

    int is_init_;

    char err_msg_[kErrMsgMaxLen];
};

#endif //NF_ZONESVR_AOI_H
