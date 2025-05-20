#ifndef _CSTACK_FLAGS_H
#define _CSTACK_FLAGS_H

#include "type.h"

enum cstack_flag_t {
    cstack_flag_none = 0x0,
    cstack_flag_verbose_tokenize = 0x1,
    cstack_flag_verbose_runtime = 0x2,
    cstack_flag_verbose_compile = 0x4,
    cstack_flag_verbose = 0x7

};

void cstack_set_flag(enum cstack_flag_t flag);
void cstack_unset_flag(enum cstack_flag_t flag);
bool_t cstack_flag(enum cstack_flag_t flag);

#endif //_CSTACK_FLAGS_H
