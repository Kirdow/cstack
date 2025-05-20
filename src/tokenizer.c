#include "tokenizer.h"

#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void push_token(struct vector_t *tokens, const char *value, enum token_type_t type)
{
    struct token_t *token = (struct token_t *)malloc(sizeof(struct token_t));
    *token = token_create(value, type);

    vector_add(tokens, (usize_t)token);
}

static struct token_t *get_token(struct vector_t *tokens, usize_t index)
{
    struct token_t *token;
    if (!vector_at(tokens, index, (usize_t *)&token)) {
        return NULL;
    }

    return token;
}

static bool_t free_token(struct vector_t *tokens, usize_t index)
{
    struct token_t *token = get_token(tokens, index);
    if (NULL == token) {
        return true;
    }

    bool_t result = token_destroy(token);

    result &= vector_erase_at(tokens, index);

    return result;
}

static bool_t free_tokens(struct vector_t *tokens)
{
    bool_t success = true;
    size_t len = (size_t)tokens->len;

    while (len > 0) {
        success &= free_token(tokens, (usize_t)(len - 1));
        
        --len;
    }
}

static char *strip_excess_whitespace(const char *str)
{
    usize_t len = strlen(str);
    char *result = (char *)malloc(sizeof(char) * (len + 1));

    char *dptr = result;
    char *last_ws_ptr = dptr;

    bool_t last_ws = false;
    for (usize_t i = 0; i < len; i++, dptr += !last_ws) {
        char c = str[i];

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            if (!last_ws) {
                *dptr = ' ';
                last_ws_ptr = dptr;
                ++dptr;
            }

            last_ws = true;
        } else {
            *dptr = c;
            last_ws = false;
        }
    }

    if (last_ws) {
        dptr = last_ws_ptr;
    }

    *dptr = '\0';

    return result;
}

static const char *next_word(const char *scan, char buffer[64])
{
    if (*scan == 0) {
        return NULL;
    }

    memset(buffer, 0, sizeof(char) * 64);

    char *ptr = buffer;
    while (*scan != ' ' && *scan != 0 && (ptr < buffer + 63)) {
        *ptr = *scan;

        ++scan;
        ++ptr;
    }

    if (*scan == ' ') {
        return scan + 1;
    } else if (*scan == 0) {
        return scan;
    } else {
        return NULL; // word was too large
    }
}

static bool_t is_number(char buffer[64])
{
    char *ptr = buffer;

    while (ptr != buffer + 63) {
        if (*ptr == 0) {
            return true;
        } else if (*ptr < '0' || *ptr > '9') {
            return false;
        }

        ++ptr;
    }

    return false;
}

static bool_t is_keyword(char buffer[64])
{
    char *ptr = buffer;

    while (ptr != buffer + 63) {
        char c = *ptr;
        if (c >= 'A' && c <= 'Z') {
            c += 32; // to lower case
        }

        if (c == 0) {
            return true;
        } else if ((c < '0' || c > '9') && (c < 'a' && c > 'z')) {
            return false;
        }

        ++ptr;
    }

    return false;
}

static enum token_type_t get_token_type(char buffer[64]) {
    if (is_number(buffer)) {
        return token_type_number;
    } else if (is_keyword(buffer)) {
        return token_type_keyword;
    } else {
        return token_type_symbol;
    }
}

bool_t tokenize_code(struct vector_t *tokens, const char *input)
{
    char *code = strip_excess_whitespace(input);

    char buffer[64];

    const char *scan = code;
    const char *next_scan = NULL;

    while ((next_scan = next_word(scan, buffer)) != NULL) {
        enum token_type_t type = get_token_type(buffer);
        push_token(tokens, buffer, type);

        scan = next_scan;
    }

    free(code);

    return false;
}

static const char *token_type_name(enum token_type_t type)
{
    switch (type)
    {
        case token_type_number: return "Number";
        case token_type_keyword: return "Keyword";
        case token_type_symbol: return "Symbol";
        default:
                                return "None";
    }
}

static char *token_format(struct token_t *token)
{
    if (NULL == token) return NULL;

    const char *type_name = token_type_name(token->type);

    if (token->type == token_type_number || token->type == token_type_keyword || token->type == token_type_symbol) {
        size_t len = strlen(type_name) + 2 + strlen(token->value);
        char *result = (char *)malloc(sizeof(char) * (len + 1));
        memset(result, 0, sizeof(char) * (len + 1));

        strncpy(result, type_name, strlen(type_name));
        strncpy(result + strlen(type_name) + 1, token->value, strlen(token->value));
        result[strlen(type_name)] = '(';
        result[len - 1] = ')';
        result[len] = '\0';

        return result;
    } else {
        size_t len = strlen(type_name);
        char *result = (char *)malloc(sizeof(char) * (len + 1));
        memset(result, 0, sizeof(char) * (len + 1));
        strncpy(result, type_name, len);
        result[len] = '\0';

        return result;
    }
}

bool_t tokenize_log(struct vector_t *tokens)
{
    if (NULL == tokens || NULL == tokens->data) {
        printf("Tokens not created\n");
        return false;
    }

    if (0 == tokens->len) {
        printf("No tokens\n");
        return true;
    }

    printf("Token Count: %lu\n", tokens->len);

    for (usize_t i = 0; i < tokens->len; i++) {
        struct token_t *token = get_token(tokens, i);
        if (NULL == token) {
            printf("%lu : Null\n", i);
            continue;
        }

        char *format = token_format(token);
        printf("%lu : %s\n", i, format);

        free(format);
    }

    return true;
}

bool_t tokenize_free(struct vector_t *tokens)
{
    return false;
}
