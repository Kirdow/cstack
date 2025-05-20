#include "token.h"

#include <string.h>
#include <stdlib.h>

struct token_t token_create(const char *value, enum token_type_t type)
{
    struct token_t result;
    memset(&result, 0, sizeof(struct token_t));
    size_t len = strlen(value);

    char *name = (char *)malloc(sizeof(char) * (len + 1));
    memset(name, 0, sizeof(char) * (len + 1));

    strncpy(name, value, len);
    name[len] = '\0';

    result.value = name;
    result.type = type;

    return result;
}

bool_t token_destroy(struct token_t *token)
{
    if (NULL == token) {
        return false;
    }

    if (NULL != token->value) {
        free(token->value);
        token->value = NULL;
    }

    memset(token, 0, sizeof(struct token_t));

    return true;
}

bool_t token_equals(struct token_t *token, const char *value, enum token_type_t type)
{
    if (NULL == token) {
        return false;
    }

    if (type != token->type) {
        return false;
    }

    if (NULL == value) {
        return true;
    }

    if (NULL == token->value) {
        return false;
    }

    return strcmp(token->value, value);
}

bool_t token_get_number_value(struct token_t *token, usize_t *result)
{
    if (NULL == token) {
        return false;
    }

    if (!token_equals(token, NULL, token_type_number)) {
        return false;
    }

    if (NULL == token->value) {
        return false;
    }

    ssize_t i = strtoll(token->value, NULL, 10);
    *result = (usize_t)i;

    return true;
}
