#include "vector_stack.h"

bool_t stack_alloc(struct stack_t *stack)
{
    if (NULL == stack) {
        return false;
    }

    return vector_alloc(&stack->vec, 8);
}

bool_t stack_release(struct stack_t *stack)
{
    if (NULL == stack) {
        return false;
    }

    return vector_release(&stack->vec);
}

bool_t stack_push(struct stack_t *stack, usize_t item)
{
    if (NULL == stack) {
        return false;
    }

    return vector_add(&stack->vec, item);
}

bool_t stack_empty(struct stack_t *stack)
{
    if (NULL == stack) {
        return true;
    }

    return stack->vec.len == 0;
}

bool_t stack_can_pop(struct stack_t *stack)
{
    if (NULL == stack) {
        return false;
    }

    return stack->vec.len > 0;
}

bool_t stack_try_pop(struct stack_t *stack, usize_t *result)
{
    if (!stack_can_pop(stack)) {
        return false;
    }
    
    usize_t item;
    if (!vector_at(&stack->vec, stack->vec.len - 1, &item)) {
        return false;
    }

    if (!vector_erase_at(&stack->vec, stack->vec.len - 1)) {
        return false;
    }

    if (NULL != result) {
        *result = item;
    }

    return true;
}

usize_t stack_pop(struct stack_t *stack, usize_t _default)
{
    usize_t item;
    if (!stack_try_pop(stack, &item)) {
        return _default;
    }

    return item;
}

usize_t stack_size(struct stack_t *stack)
{
    if (NULL == stack) {
        return 0;
    }

    return stack->vec.len;
}

