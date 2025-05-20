#include <stdio.h>

#include "source.h"
#include "tokenizer.h"
#include "string.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char *src;
    if (!source_read_file(argv[1], &src)) {
        printf("Failed to load file '%s'\n", argv[1]);
        return 1;
    }

    struct vector_t tokens;

    usize_t cap = strlen(src) / 5;
    if (cap < 8) cap = 8;

    vector_alloc(&tokens, cap);

    tokenize_code(&tokens, src);

    tokenize_log(&tokens);

    tokenize_free(&tokens);

    vector_release(&tokens);

    source_free_file(&src);

    return 0;
}

