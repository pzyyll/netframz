//
// Created by czllo on 2017/7/28.
//

#ifndef NETFRAMZ_NF_LIST_H
#define NETFRAMZ_NF_LIST_H

#include <stddef.h>

struct ListHead {
    struct ListHead *next, *prev;
};

static inline void ListHeadInit(struct ListHead *head) {
    head->prev = head;
    head->next = head;
}

static inline void ListAdd(struct ListHead *entry, struct ListHead *head) {
    entry->prev = head;
    entry->next = head->next;
    entry->next->prev = entry;
    head->next = entry;
}

static inline void ListDel(struct ListHead *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
}

#define LIST_ENTRY(ptr, type, member)  ({   \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define LIST_FOR_EACH(pos, head, member)                \
    for (pos = LIST_ENTRY((head)->next, typeof(*pos), member);  \
         &pos->member != (head);                        \
         pos = LIST_ENTRY(pos->member.next, typeof(*pos), member))

#endif //NETFRAMZ_NF_LIST_H
