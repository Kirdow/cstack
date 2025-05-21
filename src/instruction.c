#include "instruction.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hashmap.h"
#include "vector_stack.h"
#include "tokenizer.h"

struct instruction_t *instruction_create(struct token_t *token)
{
    struct instruction_t *inst = (struct instruction_t *)malloc(sizeof(struct instruction_t));
    inst->token = token;
    inst->next = -1;

    return inst;
}

void instruction_free(struct instruction_t *inst)
{
    if (NULL == inst) {
        return;
    }

    if (NULL != inst->token) {
        token_destroy(inst->token);
        inst->token = NULL;
    }

    memset(inst, 0, sizeof(struct instruction_t));
    free(inst);
}

struct vector_t *instruction_create_vector(struct vector_t *tokens)
{
    if (NULL == tokens) {
        return NULL;
    }

    struct vector_t *result = (struct vector_t *)malloc(sizeof(struct vector_t));
    vector_alloc(result, tokens->len);

    for (usize_t index = 0; index < tokens->len; index++) {
        struct token_t *token = (struct token_t *)tokens->data[index];
        struct instruction_t *inst = instruction_create(token);
        vector_add(result, (usize_t)inst);
    }

    return result;
}

void instruction_free_vector(struct vector_t *instructions)
{
    if (NULL == instructions) {
        return;
    }

    for (usize_t index = 0; index < instructions->len; index++) {
        struct instruction_t *inst = (struct instruction_t *)instructions->data[index];
        instruction_free(inst);
    }

    vector_release(instructions);
}

static bool_t instruction_scan_macros(struct vector_t *tokens, struct hashmap_t **hashmap)
{
    if (NULL == tokens || NULL == hashmap) {
        printf("Failed to scan macros. %s is null\n", (NULL == tokens) ? "Tokens" : "Hashmap");
        return false;
    }

    struct vector_t result;
    vector_alloc(&result, tokens->len);

    struct hashmap_t *map = hashmap_create();

    ssize_t macro_end = -1;

    for (usize_t index = 0; index < tokens->len; index++) {
        struct token_t *token = vector_ptr_at(tokens, index);

        if (token_equals(token, "macro", token_type_none)) {
            ++index;
            if (index >= tokens->len) {
                // TODO: Maybe free up memory in <result> items

                vector_release(&result);
                hashmap_destroy(map);

                printf("Failed to detect macro name at index %lu. Reached EOF early\n", index);

                return false;
            }

            char *macro_name = strdup(((struct token_t *)tokens->data[index])->value);

            usize_t macro_start = ++index;
            if (index >= tokens->len) {
                // TODO: Maybe free up memory in <result> items

                vector_release(&result);
                hashmap_destroy(map);

                printf("Failed to detect macro start at index %lu for macro name '%s'. Reached EOF early\n", index, macro_name);

                free(macro_name);


                return false;
            }

            ssize_t end = -1;
            ssize_t scopes = 0;

            while (index < tokens->len) {
                struct token_t *end_token = vector_ptr_at(tokens, index);

                if (token_equals(end_token, "end", token_type_none)) {
                    if (scopes == 0) {
                        end = index;
                        break;
                    } else {
                        scopes--;
                    }
                } else if (token_equals(end_token, "if", token_type_none)) {
                    ++scopes;
                } else if (token_equals(end_token, "macro", token_type_none)) {
                    // TODO: Maybe free up memory in <result> items

                    vector_release(&result);
                    hashmap_destroy(map);

                    printf("Failed to parse macro body. Detected unsupporte nested macro at index %lu for macro name '%s'\n", index, macro_name);

                    free(macro_name);

                    return false;
                }

                ++index;
            }

            if (end == -1) {
                // TODO: Maybe free up memory in <result> items

                vector_release(&result);
                hashmap_destroy(map);

                printf("Failed to find end for macro name '%s' definition at index %lu\n", macro_name, index);

                free(macro_name);

                return false;
            }

            size_t len = 0;

            for (size_t item_index = macro_start; item_index < end; item_index++) {
                if (item_index > macro_start) {
                    ++len;
                }

                struct token_t *token = vector_ptr_at(tokens, item_index);
                len += strlen(token->value);
            }

            char *macro_body = (char *)malloc(sizeof(char) * (len + 1));
            memset(macro_body, 0, sizeof(len + 1));
            char *ptr = macro_body;

            for (size_t item_index = macro_start; item_index < end; item_index++) {
                if (item_index > macro_start) {
                    *ptr = ' ';
                    ++ptr;
                }

                struct token_t *token = vector_ptr_at(tokens, item_index);

                strncpy(ptr, token->value, strlen(token->value));
                ptr += strlen(token->value);
            }

            *ptr = '\0';

            hashmap_put(map, macro_name, macro_body);
            free(macro_body);
            free(macro_name);

            macro_name = NULL;

            index = end;
        } else {
            vector_add(&result, (usize_t)token);
        }
    }

    *hashmap = map;

    vector_release(tokens);
    *tokens = result;

    return true;
}

static bool_t instruction_expand_macros(struct vector_t *tokens)
{
    if (NULL == tokens) {
        printf("Failed to expand macros. Tokens are null\n");
        return false;
    }

    struct hashmap_t *macro_map = NULL;

    if (!instruction_scan_macros(tokens, &macro_map)) {
        return false;
    }

    ssize_t dead_limit = 12;
    bool_t dirty = true;

    while (dirty && dead_limit-- > 0) {
        dirty = false;

        struct vector_t new_tokens;
        vector_alloc(&new_tokens, tokens->len * 3 / 2);

        for (usize_t index = 0; index < tokens->len; index++) {
            struct token_t *token = vector_ptr_at(tokens, index);

            const char* text = hashmap_get(macro_map, token->value);
            if (NULL != text) {
                tokenize_code(&new_tokens, text);
                dirty = true;
                continue;
            } else {
                vector_add(&new_tokens, (usize_t)token);
            }
        }

        // TODO: Maybe free token items in <tokens>

        vector_release(tokens);
        *tokens = new_tokens;
    }

    if (dead_limit < 0) {
        printf("Failed to expand macro. Dead limit reached\n");
        return false;
    }

    return true;
}

struct vector_t *instruction_process_tokens(struct vector_t *tokens)
{
    if (NULL == tokens) {
        printf("Failed to process tokens. tokens are Null\n");
        return NULL;
    }

    if (!instruction_expand_macros(tokens)) {
        return NULL;
    }

    struct stack_t ip_stack;
    stack_alloc(&ip_stack);

    struct vector_t *instructions = instruction_create_vector(tokens);

    for (usize_t index = 0; index < instructions->len; index++) {
        struct instruction_t *inst = (struct instruction_t *)instructions->data[index];
        
        if (token_equals(inst->token, "if", token_type_keyword)) {
            stack_push(&ip_stack, index);
        } else if (token_equals(inst->token, "else", token_type_keyword)) {
            ssize_t ip = (ssize_t)stack_pop(&ip_stack, (usize_t)-1);

            if (ip == -1) {
                instruction_free_vector(instructions);
                printf("Failed to process instruction. Else reached without IP-stack at index %lu\n", index);
                return NULL;
            }

            struct instruction_t *other = (struct instruction_t *)instructions->data[ip];
            if (!token_equals(other->token, "if", token_type_keyword)) {
                instruction_free_vector(instructions);
                printf("Faield to process instruction. Else reached without If at index %lu\n", index);
                return NULL;
            }

            other->next = index + 1;

            stack_push(&ip_stack, index);
        } else if (token_equals(inst->token, "end", token_type_keyword)) {
            ssize_t ip = (ssize_t)stack_pop(&ip_stack, (usize_t)-1);

            if (ip == -1) {
                instruction_free_vector(instructions);
                printf("Failed to process instruction. End reached without IP-stack at index %lu\n", index);
                return NULL;
            }

            struct instruction_t *other = (struct instruction_t *)instructions->data[ip];
            if (!token_equals(other->token, "if", token_type_keyword) && !token_equals(other->token, "else", token_type_keyword)) {
                instruction_free_vector(instructions);
                printf("Failed to process instruction. End reached without If or Else at index %lu\n", index);
                return NULL;
            }

            other->next = index + 1;
        }
    }

    if (ip_stack.vec.len > 0) {
        instruction_free_vector(instructions);
        printf("Failed to process instruction. Process ended with IP-stack size %lu. Empty required\n", ip_stack.vec.len);
        return NULL;
    }

    stack_release(&ip_stack);

    return instructions;
}
