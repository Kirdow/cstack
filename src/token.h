#ifndef _CSTACK_TOKEN_H
#define _CSTACK_TOKEN_H

#include "type.h"

enum token_type_t {
    token_type_none = 0,
    token_type_number,
    token_type_keyword,
    token_type_symbol
};

struct token_t {
    char *value;
    enum token_type_t type;
};

struct token_t token_create(const char *value, enum token_type_t type);
bool_t token_destroy(struct token_t *token);
bool_t token_equals(struct token_t *token, const char *value, enum token_type_t type);
bool_t token_get_number_value(struct token_t *token, usize_t *result);

#endif //_CSTACK_TOKEN_H
