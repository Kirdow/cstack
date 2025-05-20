#include "test.h"
#include "test_token.h"

#include "token.h"

#include <string.h>

static void test_Equals(void)
{
    struct token_t token;
    memset(&token, 0, sizeof(struct token_t));

    // Init values
    token.value = ".";
    token.type = token_type_symbol;

    // Check for type only
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, NULL, token_type_symbol), "Symbol(.) should match with (NULL, symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, NULL, token_type_keyword), "Symbol(.) should not match with (NULL, keyword)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, NULL, token_type_number), "Symbol(.) should not match with (NULL, number)");

    // Check for value only
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, ".", token_type_none), "Symbol(.) should match with (\".\", none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "+", token_type_none), "Symbol(.) should not match with (\"+\", none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "dup", token_type_none), "Symbol(.) should not match with (\"dup\", none)");

    // Check for both value and type
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, ".", token_type_symbol), "Symbol(.) should match with (\".\", symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "+", token_type_symbol), "Symbol(.) should not match with (\"+\", symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "dup", token_type_symbol), "Symbol(.) should not match with (\"dup\", symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, ".", token_type_number), "Symbol(.) should not match with (\".\", number)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "+", token_type_number), "Symbol(.) should not match with (\"+\", number)");

    // New values
    token.value = "dup";
    token.type = token_type_keyword;

    // Check for type only
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, NULL, token_type_keyword), "Keyword(dup) should match with (NULL, keyword)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, NULL, token_type_symbol), "Keyword(dup) should not match with (NULL, symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, NULL, token_type_number), "Keyword(dup) should not match with (NULL, number)");

    // Check for value only
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, "dup", token_type_none), "Keyword(dup) should match with (\"dup\", none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "2dup", token_type_none), "Keyword(dup) should not match with (\"2dup\", none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "+", token_type_none), "Keyword(dup) should not match with (\"+\", none)");

    // Check for both value and type
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, "dup", token_type_keyword), "Keyword(dup) should match with (\"dup\", keyword)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "dup", token_type_symbol), "Keyword(dup) should not match with (\"dup\", symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, ".", token_type_keyword), "Keyword(dup) should not match with (\".\", keyword)");

    // New values
    token.value = "345";
    token.type = token_type_number;

    // Check for number exact
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, "345", token_type_number), "Number(345) should match with (\"345\", number)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "346", token_type_number), "Number(345) should not match with (\"346\", number)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "345", token_type_symbol), "Number(345) should not match with (\"345\", symbol)");

    // New values
    token.value = NULL;
    token.type = token_type_symbol;

    // Check for exact match
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, NULL, token_type_symbol), "Symbol(NULL) should match with (NULL, symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "+", token_type_symbol), "Symbol(NULL) should not match with (\"+\", symbol)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "+", token_type_keyword), "Symbol(NULL) should not match with (\"+\", keyword)");

    // New values
    token.value = ".";
    token.type = token_type_none;

    // Check for exact match
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, ".", token_type_none), "None should only match with (NULL, none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "dup", token_type_none), "None should only match withh (NULL, none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, NULL, token_type_keyword), "None should only match with (NULL, none)");
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, NULL, token_type_none), "None should match with (NULL, none)");

    // New values
    token.value = ".";
    token.type = token_type_symbol;

    // Check for empty equals
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, NULL, token_type_none), "Symbol(.) should match with (NULL, none)");

    // New values
    token.value = NULL;
    token.type = token_type_none;

    // Check for exact match
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, ".", token_type_none), "None should only match with (NULL, none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, "dup", token_type_none), "None should only match withh (NULL, none)");
    TEST_ASSERT_FALSE_MESSAGE(token_equals(&token, NULL, token_type_keyword), "None should only match with (NULL, none)");
    TEST_ASSERT_TRUE_MESSAGE(token_equals(&token, NULL, token_type_none), "None should match with (NULL, none)");
}

static void test_GetNumber(void)
{
    struct token_t token;
    memset(&token, 0, sizeof(struct token_t));

    usize_t value;

    // New values
    token.type = token_type_none;
    token.value = "345";

    TEST_ASSERT_FALSE_MESSAGE(token_get_number_value(&token, &value), "Nome should not yield a number");
    
    // New values
    token.type = token_type_symbol;

    TEST_ASSERT_FALSE_MESSAGE(token_get_number_value(&token, &value), "Symbol(345) should not yield a number");

    // New values
    token.type = token_type_number;

    TEST_ASSERT_TRUE_MESSAGE(token_get_number_value(&token, &value), "Number(345) should yield a number");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(345, value, "Number(345) should yield number 345");

    // New values
    token.value = "abc";

    TEST_ASSERT_FALSE_MESSAGE(token_get_number_value(&token, &value), "Malformed Number(abc) should not yield a value");
}

void _test_token(void)
{
    RUN_TEST(test_Equals);
    RUN_TEST(test_GetNumber);
}
