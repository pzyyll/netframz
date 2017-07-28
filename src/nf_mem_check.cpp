#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "nf_mem_check.h"

using namespace std;

MemCheck::MemCheck() : size_(0) {
    ListHeadInit(&head_);
}

vois MemCheck::Add(const struct NodeInfo &info) {
    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
    if (node) {
        ListHeadInit(&node->list);
        node->info = info;
        ListAdd(&node->list, &head_);
        ++size_;
    }
}

void MemCheck::Del(void *key) {
    struct ListNode *pos;
    LIST_FOR_EACH(pos, &head, list) {
        if (pos->info.ptr == key) {
            ListDel(&pos->list);
            free(pos);
            --size_;
            break;
        }
    }
}

struct NodeInfo *MemCheck::Search(void *key) {
    struct ListNode *pos;
    LIST_FOR_EACH(pos, &head, list) {
        if (pos->info.ptr == key) {
            return &pos->info;
        }
    }

    return NULL;
}

void MemCheck::Clear() {
    struct ListHead *pos = &head_.next;
    while (pos != &head_) {
        struct ListHead *pre = pos;
        pos = pos->next;

        struct ListNode *node = LIST_ENTRY(pre, struct ListNode, list);
        ListDel(pre);
        free(node);
        --size_;
    }
}

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
