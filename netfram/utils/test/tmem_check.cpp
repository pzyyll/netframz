#include <iostream>

#define MEM_CHECK
#include "mem_check.h"

using namespace std;

int main() {
    int *pi = new int(2);
    int *p2 = new int(3);

    cout << *pi << endl;
    cout << *p2 << endl;

    delete pi;

    cout << MemCheck::GetMutableInstance().GetSize() << endl;
    for (struct NodeInfo *node = MemCheck::GetMutableInstance().GetNext();
         node != NULL;
         node = MemCheck::GetMutableInstance().GetNext()) {
        cout << node->file_name << ":" << node->line << " " << node->size << endl;
    }

    return 0;
}
