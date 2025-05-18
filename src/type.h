#ifndef _CSTACK_TYPE_H
#define _CSTACK_TYPE_H

#include <stdint.h>

typedef uint64_t usize_t;
typedef int64_t ssize_t;

typedef char bool_t;

#ifdef true
#undef true
#endif
#define true 1

#ifdef false
#undef false
#endif
#define false 0

#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0)

#endif // _CSTACK_TYPE_H
