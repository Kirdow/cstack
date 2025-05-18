#include "test.h"
#include "test_array_set.h"
#include "test_vector_list.h"
#include "type.h"

#include <string.h>

char s_buffer[512];

const char* get_test_name(const char* input)
{
    while (input[0] == '_') ++input;
    char buffer[512];
    memset(s_buffer, 0, 512);
    snprintf(buffer, 511, "%s", input);
    size_t len = strlen(buffer);
    bool_t cap = true;
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (buffer[i] == '_') {
            cap = true;
            continue;
        } else if (cap) {
            cap = false;
            if ('a' <= buffer[i] && buffer[i] <= 'z') {
                buffer[i] -= 32;
            }
        }

        s_buffer[j++] = buffer[i];
    }

    return s_buffer;
}

#define CSTACK_TEST(FN) do {\
    const char *fn_name = get_test_name(#FN);\
    UnityBegin(fn_name);\
    FN();\
    result = UnityEnd();\
    if (result != 0) {\
        return result;\
    }\
} while(0)

void setUp(void)
{}

void tearDown(void)
{}

int main()
{
    int result;

    CSTACK_TEST(_test_array_set);
    CSTACK_TEST(_test_vector_list);

    return result;
}

