#ifndef NF_MEM_CHECK_H
#define NF_MEM_CHECK_H

#include <unordered_map>
#include "nf_list.h"

struct NodeInfo {
    void *ptr;
    unsigned int size;
    unsigned int line;
    char file_name[256];
};

typedef std::unordered_map<void *, NodeInfo> mem_map_t;

class MemCheck {
public:
    struct ListNode {
        struct ListHead list;
        struct NodeInfo info;
    };

public:
    MemCheck();
    ~MemCheck();

    void Add(const struct NodeInfo &info);
    void Del(void *key);
    struct NodeInfo *Search(void *key);
    void Clear();
    unsigned long GetSize();
    struct NodeInfo *GetNext(bool is_reset = false);

private:
    struct ListHead head_;
    unsigned long size_;

    static struct ListHead &head;
    static struct ListHead &GetInstance();
};

mem_map_t &GetMemMap();
void *operator new(size_t size, const char *file, unsigned int line);
void *operator new[] (size_t size, const char *file, unsigned int line);
void operator delete(void *mem) noexcept;
void operator delete[] (void *mem) noexcept;

#endif //NF_MEM_CHECK_H
