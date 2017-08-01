#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "nf_mem_check.h"

using namespace std;

MemCheck::MemCheck() : size_(0) {
    ListHeadInit(&head_);
}

MemCheck::~MemCheck() {
    Clear();
}

void MemCheck::Add(const struct NodeInfo &info) {
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
    LIST_FOR_EACH(pos, &head_, list) {
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
    LIST_FOR_EACH(pos, &head_, list) {
        if (pos->info.ptr == key) {
            return &pos->info;
        }
    }

    return NULL;
}

void MemCheck::Clear() {
    struct ListHead *pos = head_.next;
    while (pos != &head_) {
        struct ListHead *pre = pos;
        pos = pos->next;

        struct ListNode *node = LIST_ENTRY(pre, struct ListNode, list);
        ListDel(pre);
        free(node);
        --size_;
    }
}

unsigned long MemCheck::GetSize() {
    return size_;
}

struct NodeInfo *MemCheck::GetNext(bool is_reset) {
    static struct ListHead *pos = head_.next;

    if (is_reset)
        pos = head_.next;
    else
        pos = pos->next;

    if (pos != &head_)
        return &(LIST_ENTRY(pos, struct ListNode, list)->info);

    return NULL;
}

MemCheck &MemCheck::GetMutableInstance() {
    return instance;
}

MemCheck &MemCheck::GetInstance() {
    static MemCheck mem_check;
    (void)(mem_check);
    return mem_check;
}

MemCheck &MemCheck::instance = MemCheck::GetInstance();

void *operator new(size_t size, const char *file, unsigned int line) {
    void *mem = malloc(size);
    if (mem) {
        NodeInfo info;
        info.ptr = mem;
        info.size = size;
        info.line = line;
        snprintf(info.file_name, sizeof(info.file_name), file);
        MemCheck::GetMutableInstance().Add(info);
    }

    return mem;
}

void *operator new[] (size_t size, const char *file, unsigned int line) {
    return (operator new(size, file, line));
}

void operator delete(void *mem) noexcept {
    MemCheck::GetMutableInstance().Del(mem);
    free(mem);
}

void operator delete[] (void *mem) noexcept {
    operator delete(mem);
}
