#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "nf_mem_check.h"

using namespace std;

static void use(mem_map_t const *m) {  }

mem_map_t &GetMemMap() {
    static mem_map_t map;
    use(&map);
    return map;
}

void *operator new(size_t size, const char *file, unsigned int line) {
    void *mem = malloc(size);
    if (mem) {
        NodeInfo info;
        info.size = size;
        info.line = line;
        snprintf(info.file_name, sizeof(info.file_name), file);
        GetMemMap().insert(std::make_pair(mem, info));
    }

    return mem;
}

void *operator new[] (size_t size, const char *file, unsigned int line) {
    return (operator new(size, file, line));
}

void operator delete(void *mem) noexcept {
    free(mem);

    mem_map_t::iterator itr = GetMemMap().find(mem);
    if (itr != GetMemMap().end()) {
        GetMemMap().erase(itr);
    }
}

void operator delete[] (void *mem) noexcept {
    operator delete(mem);
}
