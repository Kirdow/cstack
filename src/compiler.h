#ifndef _CSTACK_COMPILER_H
#define _CSTACK_COMPILER_H

#include "type.h"
#include "vector_list.h"

typedef usize_t comp_id;

comp_id compiler_init(struct vector_t *tokens);

bool_t compiler_exec(comp_id id, const char *binary_path);

bool_t compiler_release(comp_id id);

#endif //_CSTACK_COMPILER_H
