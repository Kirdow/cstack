#ifndef _CSTACK_TOKENIZER_H
#define _CSTACK_TOKENIZER_H

#include "type.h"
#include "vector_list.h"

bool_t tokenize_code(struct vector_t *tokens, const char *input);
bool_t tokenize_log(struct vector_t *tokens);
bool_t tokenize_free(struct vector_t *tokens);

#endif //_CSTACK_TOKENIZER_H
