#include <stdio.h>

#include "vector_stack.h"

int main(int argc, char** argv)
{
    printf("\nStack demo\n\n");

    usize_t value;
    struct stack_t stack;
    stack_alloc(&stack);

    printf("First push : 5\n");
    stack_push(&stack, 5);

    printf("Second push : 7\n");
    stack_push(&stack, 7);

    printf("Third push : 19\n\n");
    stack_push(&stack, 19);
    
    printf("Stack size : %lu\n\n", stack_size(&stack));

    printf("First pop : %ld\n", (ssize_t)stack_pop(&stack, -1));
    printf("Second pop : %ld\n", (ssize_t)stack_pop(&stack, -1));
    printf("Third pop : %ld\n", (ssize_t)stack_pop(&stack, -1));
    printf("Fourth pop : %ld\n\n", (ssize_t)stack_pop(&stack, -1));

    stack_release(&stack);

    return 0;
}

