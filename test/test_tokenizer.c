#include "test.h"
#include "test_tokenizer.h"

#include "tokenizer.h"
#include "token.h"

#include <string.h>

#define PREP(S) do{\
    memset(buffer, 0, sizeof(char) * 64);\
    strncpy(buffer, S, 63);\
    buffer[63] = '\0';\
} while(0)

bool_t is_number(char buffer[64]);
bool_t is_keyword(char buffer[64]);
bool_t is_symbol(char buffer[64]);
enum token_type_t get_token_type(char buffer[64]);

void test_Number(void)
{
    char buffer[64];

    PREP("555");

    TEST_ASSERT_TRUE_MESSAGE(is_number(buffer), "555 should only resolve as number type");
    TEST_ASSERT_FALSE_MESSAGE(is_keyword(buffer), "555 should only resolve as number type");
    TEST_ASSERT_FALSE_MESSAGE(is_symbol(buffer), "555 should only resolve as number type");
}

void test_Keyword(void)
{
    char buffer[64];

    PREP("d55");

    TEST_ASSERT_FALSE_MESSAGE(is_number(buffer), "d55 should only resolve as keyword type");
    TEST_ASSERT_TRUE_MESSAGE(is_keyword(buffer), "d55 should only resolve as keyword type");
    TEST_ASSERT_FALSE_MESSAGE(is_symbol(buffer), "d55 should only resolve as keyword type");

    PREP("5d5");

    TEST_ASSERT_FALSE_MESSAGE(is_number(buffer), "5d5 should only resolve as keyword type");
    TEST_ASSERT_TRUE_MESSAGE(is_keyword(buffer), "5d5 should only resolve as keyword type");
    TEST_ASSERT_FALSE_MESSAGE(is_symbol(buffer), "5d5 should only resolve as keyword type");

    PREP("2dup");

    TEST_ASSERT_FALSE_MESSAGE(is_number(buffer), "2dup should only resolve as keyword type");
    TEST_ASSERT_TRUE_MESSAGE(is_keyword(buffer), "2dup should only resolve as keyword type");
    TEST_ASSERT_FALSE_MESSAGE(is_symbol(buffer), "2dup should only resolve as keyword type");
}

void test_Symbol(void)
{
    char buffer[64];

    PREP(".");

    TEST_ASSERT_FALSE_MESSAGE(is_number(buffer), ". should only resolve as symbol type");
    TEST_ASSERT_FALSE_MESSAGE(is_keyword(buffer), ". should only resolve as symbol type");
    TEST_ASSERT_TRUE_MESSAGE(is_symbol(buffer), ". should only resolve as symbol type");

    PREP("+");

    TEST_ASSERT_FALSE_MESSAGE(is_number(buffer), "+ should only resolve as symbol type");
    TEST_ASSERT_FALSE_MESSAGE(is_keyword(buffer), "+ should only resolve as symbol type");
    TEST_ASSERT_TRUE_MESSAGE(is_symbol(buffer), "+ should only resolve as symbol type");
}

void test_TokenType(void)
{
    char buffer[64];

    PREP("555");

    TEST_ASSERT_EQUAL_INT_MESSAGE(token_type_number, get_token_type(buffer), "555 should only resolve as token_type_number");

    PREP("d55");

    TEST_ASSERT_EQUAL_INT_MESSAGE(token_type_keyword, get_token_type(buffer), "d55 should only resolve as token_type_keyword");

    PREP("5d5");

    TEST_ASSERT_EQUAL_INT_MESSAGE(token_type_keyword, get_token_type(buffer), "5d5 should only resolve as token_type_keyword");

    PREP("2dup");

    TEST_ASSERT_EQUAL_INT_MESSAGE(token_type_keyword, get_token_type(buffer), "2dup should only resolve as token_type_keyword");

    PREP(".");

    TEST_ASSERT_EQUAL_INT_MESSAGE(token_type_symbol, get_token_type(buffer), ". should only resolve as token_type_symbol");

    PREP("+");

    TEST_ASSERT_EQUAL_INT_MESSAGE(token_type_symbol, get_token_type(buffer), "+ should only resolve as token_type_symbol");
}

void _test_tokenizer(void)
{
    RUN_TEST(test_Number);
    RUN_TEST(test_Keyword);
    RUN_TEST(test_Symbol);
    RUN_TEST(test_TokenType);
}
