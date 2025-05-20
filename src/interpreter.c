#include "interpreter.h"
#include "tokenizer.h"
#include "token.h"

#include "vector_stack.h"

#include "flags.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

struct interpreter_instance {
    struct stack_t *program_stack;
    struct vector_t *program_code;
    usize_t ip;
    usize_t eop;
};

#define INST(X) ((struct interpreter_instance *)(X))
#define ID(X) ((inter_id)(X))

inter_id interpreter_init(struct vector_t *tokens)
{
    // Allocate memory for instance
    struct interpreter_instance *instance = (struct interpreter_instance *)malloc(sizeof(struct interpreter_instance));
    if (NULL == instance) {
        return 0;
    }

    // Allocate memory for stack and code
    instance->program_stack = (struct stack_t *)malloc(sizeof(struct stack_t));
    instance->program_code = (struct vector_t *)malloc(sizeof(struct vector_t));

    // Take ownership of code
    *instance->program_code = *tokens;
    // Zero-Memory on the caller's copy
    memset(tokens, 0, sizeof(struct vector_t));

    // Create the stack
    stack_alloc(instance->program_stack);

    // Set EOP (End-Of-Program) to code length
    instance->eop = instance->program_code->len;

    // Set IP (Instruction Pointer) to code start (aka zero)
    instance->ip = 0;

    return ID(instance);
}

bool_t interpreter_release(inter_id id)
{
    if (0 == id) {
        return false;
    }

    struct interpreter_instance *instance = INST(id);

    // Release the stack
    stack_release(instance->program_stack);
    // Don't forget to free the stack object itself
    free(instance->program_stack);
    instance->program_stack = NULL;


    // Release the code
    tokenize_free(instance->program_code);
    // Don't forget to free the code
    free(instance->program_code);
    instance->program_code = NULL;

    // Reset ip and eop
    instance->ip = 0;
    instance->eop = 0;

    // Free the instance object
    free(instance);

    return true;
}

bool_t interpreter_run(inter_id id)
{
    if (0 == id) {
        return false;
    }

    struct interpreter_instance *instance = INST(id);

    while (instance->ip < instance->eop) {
        if (!interpreter_step(id)) {
            return false;
        }

        // We assume that the step itself increment or modify IP for us. This could be due to IF, WHILE, ELSE etc where we may jump back and fourth. We don't want to interfere with those situations.
    }

    return true;
}

struct token_t *get_token(struct vector_t *tokens, usize_t index);

static bool_t runtimev(const char *format, ...)
{
    if (!cstack_flag(cstack_flag_verbose_runtime)) {
        return false;
    }

    const char *color_yellow = "\033[33m";
    const char *color_reset = "\033[0m";

    size_t runtime_len = strlen("Runtime: ");
    size_t yellow_len = strlen(color_yellow);
    size_t reset_len = strlen(color_reset);
    size_t format_len = strlen(format);

    size_t len = runtime_len + yellow_len + reset_len + format_len;
    char *buffer= (char *)malloc(sizeof(char) * (len + 1));

    memset(buffer, 0, sizeof(char) * (len + 1));
    strncpy(buffer, color_yellow, yellow_len);
    strncpy(buffer + yellow_len, "Runtime: ", runtime_len);
    strncpy(buffer + yellow_len + runtime_len, color_reset, reset_len);
    strncpy(buffer + yellow_len + runtime_len + reset_len, format, format_len);
    buffer[len] = '\0';

    va_list args;
    va_start(args);
    vprintf(buffer, args);
    va_end(args);

    free(buffer);

    return false;
}

char *token_format(struct token_t *token);

static ssize_t ipop(struct stack_t *stack)
{
    ssize_t result = (ssize_t)stack_pop(stack, (usize_t)0);
    runtimev("New stack size %lu\n", stack->vec.len);
    return result;
}

static ssize_t ipeek_offset(struct stack_t *stack, usize_t offset)
{
    return (ssize_t)stack->vec.data[stack->vec.len - 1 - offset];
}

static ssize_t ipeek(struct stack_t *stack)
{
    return ipeek_offset(stack, 0);
}

static void ipush(struct stack_t *stack, ssize_t item)
{
    stack_push(stack, (usize_t)item);
    runtimev("New stack size %lu\n", stack->vec.len);
}

bool_t interpreter_step(inter_id id)
{
    if (0 == id) {
        return runtimev("Failed to run step. inter_id is 0\n");
    }

    struct interpreter_instance *instance = INST(id);

    // Make sure IP is within EOP
    if (instance->ip >= instance->eop) {
        return runtimev("Failed to run step. inter_id is 0\n");
    }

    // Get current token
    struct token_t *token = get_token(instance->program_code, instance->ip);

    // If token is NULL, return error
    if (NULL == token) {
        return runtimev("Failed to run step. token at index %lu is Null\n", instance->ip);
    }

    runtimev("Current stack size %lu\n", instance->program_stack->vec.len);
    if (cstack_flag(cstack_flag_verbose_runtime)) {
        char *token_log = token_format(token);
        if (token_log) {
            runtimev("Current token [%s] at IP %lu\n", token_log, instance->ip);
            free(token_log);
        }
    }
    if (token_equals(token, NULL, token_type_number)) {
        ssize_t num;
        token_get_number_value(token, (usize_t *)&num);

        runtimev("Pushing %ld\n", num);
        ipush(instance->program_stack, num);
    } else if (token_equals(token, ".", token_type_none)) {
        if (!stack_can_pop(instance->program_stack)) {
            return runtimev("Failed to runs tep. log operator failed. Stack is empty.\n");
        }

        ssize_t value = ipop(instance->program_stack);
        printf("%ld\n", value);
    } else if (token_equals(token, "+", token_type_symbol)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. + operator failed. Stack size is %lu. 2 is required.\n", instance->program_stack->vec.len);
        }

        runtimev("Adding two numbers.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);

        ssize_t result = n1 + n2;

        ipush(instance->program_stack, result);
        runtimev("Pushed %ld\n", result);
    } else if (token_equals(token, "-", token_type_symbol)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. - operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Subtracting two numbers.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);

        ssize_t result = n2 - n1;

        ipush(instance->program_stack, result);
        runtimev("Pushed %ld\n", result);
    } else if (token_equals(token, "*", token_type_symbol)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. * operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Multiplying two numbers.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);

        ssize_t result = n2 * n1;

        ipush(instance->program_stack, result);
        runtimev("Pushed %ld\n", result);
    } else if (token_equals(token, "/", token_type_symbol)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. / operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Dividing two numbers.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);

        if (n1 == 0) {
            runtimev("Warning: dividend is zero\n");
        }

        ssize_t result = (n1 == 0) ? 0 : (n2 / n1);

        ipush(instance->program_stack, result);
        runtimev("Pushed %ld\n", result);
    } else if (token_equals(token, "%", token_type_symbol)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. % operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Modulo two numbers.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);

        if (n1 == 0) {
            runtimev("Warning: dividend is zero\n");
        }

        ssize_t result = (n1 == 0) ? 0 : (n2 % n1);

        ipush(instance->program_stack, result);
        runtimev("Pushed %ld\n", result);
    } else if (token_equals(token, "dup", token_type_keyword)) {
        ssize_t n1;

        if (instance->program_stack->vec.len < 1) {
            return runtimev("Failed to run step. dup operator failed. Stack is empty\n");
        }

        runtimev("Duplicate top stack value.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        ipush(instance->program_stack, n1);
        runtimev("Pushed %ld\n", n1);
        ipush(instance->program_stack, n1);
        runtimev("Pushed %ld\n", n1);
    } else if (token_equals(token, "swap", token_type_keyword)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. swap operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Swapping top two stack values.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);
        ipush(instance->program_stack, n1);
        runtimev("Pushed %ld\n", n1);
        ipush(instance->program_stack, n2);
        runtimev("Pushed %ld\n", n2);
    } else if (token_equals(token, "over", token_type_keyword)) {
        ssize_t n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. over operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Pushing 2nd top-most value onto stack.\n");
        n2 = ipeek_offset(instance->program_stack, 1);
        runtimev("Peeked(1) %ld\n", n2);
        ipush(instance->program_stack, n2);
        runtimev("Pushed %ld\n", n2);
    } else if (token_equals(token, "2dup", token_type_keyword)) {
        ssize_t n1, n2;

        if (instance->program_stack->vec.len < 2) {
            return runtimev("Failed to run step. 2dup operator failed. Stack size is %lu. 2 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Two-duping top stack values.\n");
        n1 = ipeek_offset(instance->program_stack, 1);
        runtimev("Peeked(1) %ld\n", n1);
        n2 = ipeek_offset(instance->program_stack, 0);
        runtimev("Peeked(0) %ld\n", n2);
        ipush(instance->program_stack, n1);
        runtimev("Pushed %ld\n", n1);
        ipush(instance->program_stack, n2);
        runtimev("Pushed %ld\n", n2);
    } else if (token_equals(token, "2swap", token_type_keyword)) {
        ssize_t n1, n2, n3, n4;

        if (instance->program_stack->vec.len < 4) {
            return runtimev("Failed to run step. 2swap operator failed. Stack size is %lu. 4 is required\n", instance->program_stack->vec.len);
        }

        runtimev("Two-swapping top stack values.\n");
        n1 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n1);
        n2 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n2);
        n3 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n3);
        n4 = ipop(instance->program_stack);
        runtimev("Popped %ld\n", n4);

        ipush(instance->program_stack, n2);
        runtimev("Pushed %ld\n", n2);
        ipush(instance->program_stack, n1);
        runtimev("Pushed %ld\n", n1);
        ipush(instance->program_stack, n4);
        runtimev("Pushed %ld\n", n4);
        ipush(instance->program_stack, n3);
        runtimev("Pushed %ld\n", n3);
    } else {
        if (cstack_flag(cstack_flag_verbose_runtime)) {
            char *token_log = token_format(token);
            if (token_log) {
                runtimev("Invalid token [%s] at IP %lu\n", token_log, instance->ip);
                free(token_log);
            } else {
                runtimev("Invalid token at IP %lu\n", instance->ip);
            }
        }

        return false;
    }

    // For a normal step, increment the IP
    ++instance->ip;

    return true;
}
