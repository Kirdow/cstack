#include <stdio.h>

#include "source.h"
#include "tokenizer.h"
#include "string.h"
#include "interpreter.h"
#include "compiler.h"

#include "flags.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: %s [options] <file>\n", argv[0]);
        return 1;
    }

    const char *path = NULL;
    const char *bin_path = NULL;

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
        } else if (strncmp(argv[argi], "-o", 2) == 0) {
            argi++;
            if (argi >= argc) {
                printf("Not enough arguments for binary path\n");
                return 1;
            }

            bin_path = argv[argi];
        } else if (NULL == path) {
            path = argv[argi];
        } else {
            printf("More than one file provided.\nUsage: %s [options] <file>\n", argv[0]);
            return 1;
        }
    }

    char *src;
    if (!source_read_file(path, &src)) {
        printf("Failed to load file '%s'\n", path);
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

    if (bin_path) {
        comp_id compiler_id = compiler_init(&tokens);
        if (0 == compiler_id) {
            printf("Failed to initialize compiler\n");
            return 1;
        }

        if (!compiler_exec(compiler_id, bin_path)) {
            printf("Program could not be compiled\n");

            if (!compiler_release(compiler_id)) {
                printf("Error releasing compiler after compile issue\n");
            }

            return 1;
        }

        if (!compiler_release(compiler_id)) {
            printf("Program exit incorrectly. Failed to release compiler\n");
            return 1;
        }

        return 0;
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

