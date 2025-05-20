#include "flags.h"

static enum cstack_flag_t s_flags = 0;

void cstack_set_flag(enum cstack_flag_t flag)
{
    s_flags |= flag;
}

void cstack_unset_flag(enum cstack_flag_t flag)
{
    s_flags &= ~flag;
}

bool_t cstack_flag(enum cstack_flag_t flag)
{
    return (s_flags & flag) != 0;
}
