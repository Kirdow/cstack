#include <stdio.h>

#include "source.h"
#include "tokenizer.h"
#include "string.h"
#include "interpreter.h"

#include "flags.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: %s [options] <file>\n", argv[0]);
        return 1;
    }

    const char *path = NULL;

    for (int argi = 1; argi < argc; argi++) {
        if (strncmp(argv[argi], "--", 2) == 0) {
            if (strcmp(argv[argi], "--verbose-runtime") == 0) {
                cstack_set_flag(cstack_flag_verbose_runtime);
            } else if (strcmp(argv[argi], "--verbose-compile") == 0) {
                cstack_set_flag(cstack_flag_verbose_compile);
            } else if (strcmp(argv[argi], "--verbose-tokenize") == 0) {
                cstack_set_flag(cstack_flag_verbose_tokenize);
            } else if (strcmp(argv[argi], "--verbose") == 0) {
                cstack_set_flag(cstack_flag_verbose);
            }
        } else if (NULL == path) {
            path = argv[argi];
        } else {
            printf("More than one file provided.\nUsage: %s [options] <file>\n", argv[0]);
            return 1;
        }
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
    source_free_file(&src);

    if (cstack_flag(cstack_flag_verbose_tokenize)) {
        tokenize_log(&tokens);
    }

    inter_id runtime_id = interpreter_init(&tokens); 
    if (0 == runtime_id) {
        printf("Failed to initialize interpreter\n");
        return 1;
    }

    if (!interpreter_run(runtime_id)) {
        printf("Program ran into an error interpreting\n");

        if (!interpreter_release(runtime_id)) {
            printf("Error releasing interpreter after execution issue\n");
        }

        return 1;
    }

    if (!interpreter_release(runtime_id)) {
        printf("Program exit incorrectly. Failed to release interpreter\n");
        return 1;
    }

    return 0;
}

