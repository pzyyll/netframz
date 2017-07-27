#ifndef MEM_CHECK_H
#define MEM_CHECK_H

#ifdef MEM_CHECK
#include "nf_mem_check.h"
#define new new(__FILE__, __LINE__)
#endif //MEM_CHECK

#endif //MEM_CHECK_H
