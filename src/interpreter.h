#ifndef _CSTACK_INTERPRETER_H
#define _CSTACK_INTERPRETER_H

#include "type.h"
#include "vector_list.h"

typedef usize_t inter_id;

// This one takes ownership of <tokens>
inter_id interpreter_init(struct vector_t *tokens);

// Releases interpreter. This frees up the vector passed into init
bool_t interpreter_release(inter_id id);

// Runs one instruction
bool_t interpreter_step(inter_id id);

// Runs until exit or all instructions are consumed
bool_t interpreter_run(inter_id id);

#endif //_CSTACK_INTERPRETER_H
