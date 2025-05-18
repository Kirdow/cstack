#include "test.h"
#include "test_array_set.h"

#include "array_set.h"

static void test_GeneralUse(void)
{
    struct array_set_t set;
    array_set_alloc(&set, 8);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(8, set.capacity, "Make sure capacity is set to 8");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, set.len, "Make sure initial length is set to 0");

    array_set_add(&set, 69);
    array_set_add(&set, 420);
    array_set_add(&set, 17);
    array_set_add(&set, 9);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(4, set.len, "Make sure length is 4 after inserting 4 values");

    array_set_erase(&set, 420);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(3, set.len, "Make sure length is 3 after removing one from previous 4 length");

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(69, set.data[0], "First item in array_set");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(9, set.data[1], "Second item in array_set");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(17, set.data[2], "Third item in array_set");

    for (int i = 3; i < 8; i++) {
        array_set_add(&set, i);
        char buffer[64];
        snprintf(buffer, 63, "Added item #%d in array_set", i); 
        TEST_ASSERT_EQUAL_UINT64_MESSAGE(i, set.data[i], buffer);
    }

    bool_t success = array_set_add(&set, 8);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(8, set.len, "Length unchanged on array_set at capacity");

    TEST_ASSERT_FALSE_MESSAGE(success, "Result should be false when adding to array_set at capacity");

    array_set_erase(&set, 7);
    array_set_erase(&set, 17);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(9, set.data[1], "Second item after removing two in array_set");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, set.data[2], "Third item after removing two in array_set");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, set.len, "Length of array_set after removing 2 from capacity");

    array_set_clear(&set);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, set.len, "array_set empty after clear");

    array_set_release(&set);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, set.data, "array_set data should be NULL after release");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, set.len, "array_set length should be 0 after release");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, set.capacity, "array_set capacity should be 0 after release");
}

void _test_array_set(void)
{
    RUN_TEST(test_GeneralUse);
}
