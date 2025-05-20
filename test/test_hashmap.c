#include "test.h"
#include "test_hashmap.h"

#include "hashmap.h"

static void test_Create(void)
{
    struct hashmap_t *hashmap= hashmap_create();

    TEST_ASSERT_NOT_NULL_MESSAGE(hashmap, "Hashmap creation should not fail");

    hashmap_destroy(hashmap);
}

static void test_Put(void)
{
    struct hashmap_t *hashmap = hashmap_create();
    
    // Make sure we can put 5 fields
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "Alice", "Cooper"), "Put should not fail");
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "Bob", "Marley"), "Put should not fail");
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "Peter", "Parker"), "Put should not fail");
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "Gwen", "Stacy"), "Put should not fail");
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "David", "Bowie"), "Put should not fail");

    // Make sure we can override fields
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "Bob", "Dylan"), "Put-override should not fail");
    TEST_ASSERT_TRUE_MESSAGE(hashmap_put(hashmap, "Peter", "Jackson"), "Put-override should not fail");

    hashmap_destroy(hashmap);
}

static void test_Get(void)
{
    struct hashmap_t *hashmap = hashmap_create();

    hashmap_put(hashmap, "Alice", "Cooper");
    hashmap_put(hashmap, "Bob", "Marley");
    hashmap_put(hashmap, "Peter", "Parker");
    hashmap_put(hashmap, "Gwen", "Stacy");
    hashmap_put(hashmap, "David", "Bowie");

    const char *result;

    // Test getting existing value
    result = hashmap_get(hashmap, "Bob");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting existing value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Marley", result, "Getting existing value should match expected value");

    // Test another existing value
    result = hashmap_get(hashmap, "Peter");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting existing value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Parker", result, "Getting existing value should match expected value");

    // Test non-existing value
    result = hashmap_get(hashmap, "John");
    TEST_ASSERT_NULL_MESSAGE(result, "Getting non-existing value should yield null");

    // Modify values
    hashmap_put(hashmap, "Bob", "Dylan");
    hashmap_put(hashmap, "Peter", "Jackson");

    // Test getting overwritten value
    result = hashmap_get(hashmap, "Bob");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting overwritten value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Dylan", result, "Getting overwritten value should match expected value");

    // Test getting another overwritten value
    result = hashmap_get(hashmap, "Peter");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting overwritten value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Jackson", result, "Getting overwritten value should match expected value");

    // Test getting non-overwritten value
    result = hashmap_get(hashmap, "David");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting non-overwritten value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Bowie", result, "Getting non-overwritten value should match expected value");

    // Test getting non-existing value
    result = hashmap_get(hashmap, "John");
    TEST_ASSERT_NULL_MESSAGE(result, "Getting non-existing value should yield null");

    hashmap_destroy(hashmap);
}

static void test_Remove(void)
{
    struct hashmap_t *hashmap = hashmap_create();

    hashmap_put(hashmap, "Alice", "Cooper");
    hashmap_put(hashmap, "Bob", "Dylan");

    const char *result;
    bool_t success;

    // Test getting existing value
    result = hashmap_get(hashmap, "Bob");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting existing value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Dylan", result, "Getting existing value should match expected value");

    // Remove value
    success = hashmap_remove(hashmap, "Bob");

    // Assert removal
    TEST_ASSERT_TRUE_MESSAGE(success, "Removing existing value should not fail");

    // Test getting removed value
    result = hashmap_get(hashmap, "Bob");
    TEST_ASSERT_NULL_MESSAGE(result, "Getting removed value should yield null"); 

    // Test getting not-removed value
    result = hashmap_get(hashmap, "Alice");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting not-removed value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Cooper", result, "Getting not-removed value should match expected value");

    // Re-add value
    success = hashmap_put(hashmap, "Bob", "Marley");

    // Assert re-adding
    TEST_ASSERT_TRUE_MESSAGE(success, "Re-adding value should not fail");

    // Test getting re-added value
    result = hashmap_get(hashmap, "Bob");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting re-added value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Marley", result, "Getting re-added value should match expected value");

    // Modify re-added value
    success = hashmap_put(hashmap, "Bob", "Dylan");

    // Assert modification
    TEST_ASSERT_TRUE_MESSAGE(success, "Modifying re-added value should not fail");

    // Test getting modified value
    result = hashmap_get(hashmap, "Bob");
    TEST_ASSERT_NOT_NULL_MESSAGE(result, "Getting modified re-added value should not yield null");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Dylan", result, "Getting modified re-added value should match expected value");

    // Try removing non-existing value
    success = hashmap_remove(hashmap, "David");

    // Assert removing non-existing value fails
    TEST_ASSERT_FALSE_MESSAGE(success, "Removing non-existing value should fail");
}

void _test_hashmap(void)
{
    RUN_TEST(test_Create);
    RUN_TEST(test_Put);
    RUN_TEST(test_Get);
    RUN_TEST(test_Remove);
}
