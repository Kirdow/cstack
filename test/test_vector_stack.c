#include "test.h"
#include "test_vector_stack.h"

#include "vector_stack.h"

static void test_Alloc(void)
{
    struct stack_t stack;
    stack_alloc(&stack);

    TEST_ASSERT_NOT_NULL_MESSAGE(stack.vec.data, "Stack should have a data pointer after creation");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(8, stack.vec.capacity, "Capacity for stack should start at 8");

    for (usize_t i = 0; i < 8; i++) {
        stack_push(&stack, (i + 1) * 10);
    }

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(8, stack.vec.capacity, "Capacity for stack should stay at 8 after 8 pushes");

    stack_push(&stack, 12);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(12, stack.vec.capacity, "Capacity for stack should be 12 after first overflow");

    stack_release(&stack);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.capacity, "Capacity should be zero after release");
    TEST_ASSERT_NULL_MESSAGE(stack.vec.data, "Stack should have no data pointer after release");
}

static void test_Push(void)
{
    struct stack_t stack;
    stack_alloc(&stack);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.len, "Length should be zero before any pushes");

    stack_push(&stack, 1);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(1, stack.vec.len, "Length should be one after first push");

    stack_push(&stack, 2);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(2, stack.vec.len, "Length should be two after second push");

    stack_push(&stack, 3);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(3, stack.vec.len, "Length should be three after third push");

    stack_release(&stack);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.len, "Length should be zero after release");
}

static void test_Pop(void)
{
    usize_t value;
    struct stack_t stack;
    stack_alloc(&stack);

    for (usize_t i = 0; i < 3; i++) {
        stack_push(&stack, (i + 1) * 10);
    }

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(3, stack.vec.len, "Length should be three after initial pushes");

    value = stack_pop(&stack, (usize_t)-1);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(2, stack.vec.len, "Length should be two after first pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(30, value, "Value should be 30 after first pop");

    value = stack_pop(&stack, (usize_t)-1);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(1, stack.vec.len, "Length should be one after second pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(20, value, "Value should be 20 after second pop");

    value = stack_pop(&stack, (usize_t)-1);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.len, "Length should be zero after third pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(10, value, "Value should be 10 after third pop");

    value = stack_pop(&stack, (usize_t)-1);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.len, "Length should be zero after fourth pop");
    TEST_ASSERT_EQUAL_INT64_MESSAGE((int64_t)-1, (int64_t)value, "Value should be -1 after fourth pop");

    stack_release(&stack);
}

static void test_TryPop(void)
{
    usize_t value;
    bool_t success;
    struct stack_t stack;
    stack_alloc(&stack);

    for (usize_t i = 0; i < 3; i++) {
        stack_push(&stack, (i + 1) * 10);
    }

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(3, stack.vec.len, "Length should be three after initial pushes");

    success = stack_try_pop(&stack, &value);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(2, stack.vec.len, "Length should be two after first pop");
    TEST_ASSERT_TRUE_MESSAGE(success, "Success should be true after first pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(30, value, "Value should be 30 after first pop");

    success = stack_try_pop(&stack, &value);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(1, stack.vec.len, "Length should be one after second pop");
    TEST_ASSERT_TRUE_MESSAGE(success, "Success should be true after second pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(20, value, "Value should be 20 after second pop");

    success = stack_try_pop(&stack, &value);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.len, "Length should be zero after third pop");
    TEST_ASSERT_TRUE_MESSAGE(success, "Success should be true after third pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(10, value, "Value should be 10 after third pop");

    success = stack_try_pop(&stack, &value);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, stack.vec.len, "Length should be zero after fourth pop");
    TEST_ASSERT_FALSE_MESSAGE(success, "Success should be false after fourth pop");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(10, value, "Value should be untouched after fourth pop");

    stack_release(&stack);
}

void _test_vector_stack(void)
{
    RUN_TEST(test_Alloc);
    RUN_TEST(test_Push);
    RUN_TEST(test_Pop);
    RUN_TEST(test_TryPop);
}
