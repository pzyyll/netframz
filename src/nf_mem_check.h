#ifndef NF_MEM_CHECK_H
#define NF_MEM_CHECK_H

#include <unordered_map>

struct NodeInfo {
    unsigned int size;
    unsigned int line;
    char file_name[256];
};

typedef std::unordered_map<void *, NodeInfo> mem_map_t;

mem_map_t &GetMemMap();
void *operator new(size_t size, const char *file, unsigned int line);
void *operator new[] (size_t size, const char *file, unsigned int line);
void operator delete(void *mem) noexcept;
void operator delete[] (void *mem) noexcept;

#endif //NF_MEM_CHECK_H
