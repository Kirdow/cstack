#ifndef _CSTACK_INSTRUCTION_H
#define _CSTACK_INSTRUCTION_H

#include "token.h"
#include "vector_list.h"

struct instruction_t {
    struct token_t *token;
    ssize_t next;
};

struct instruction_t *instruction_create(struct token_t *token);
void instruction_free(struct instruction_t *inst);

struct vector_t *instruction_create_vector(struct vector_t *tokens);
void instruction_free_vector(struct vector_t *instructions);

struct vector_t *instruction_process_tokens(struct vector_t *tokens);

#endif //_CSTACK_INSTRUCTION_H
