#include "compiler.h"
#include "token.h"
#include "util.h"

#include "flags.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

struct compiler_instance {
    struct vector_t *program_code;
};

#include "instruction.h"

#define INST(X) ((struct compiler_instance *)(X))
#define ID(X) ((comp_id)(X))

comp_id compiler_init(struct vector_t *tokens)
{
    // Allocate memory for instance
    struct compiler_instance *instance = (struct compiler_instance *)malloc(sizeof(struct compiler_instance));
    if (NULL == instance) {
        return 0;
    }

    // Pre-process tokens and get instructions
    instance->program_code = instruction_process_tokens(tokens);

    // Validate creation of instructions
    if (NULL == instance->program_code) {
        free(instance);
        return 0;
    }

    memset(tokens, 0, sizeof(struct vector_t));

    return ID(instance);
}

static bool_t compilev(const char *format, ...)
{
    if (!cstack_flag(cstack_flag_verbose_compile)) {
        return false;
    }

    const char *color_yellow = "\033[33m";
    const char *color_reset = "\033[0m";

    size_t runtime_len = strlen("Compiler: ");
    size_t yellow_len = strlen(color_yellow);
    size_t reset_len = strlen(color_reset);
    size_t format_len = strlen(format);

    size_t len = runtime_len + yellow_len + reset_len + format_len;
    char *buffer= (char *)malloc(sizeof(char) * (len + 1));

    memset(buffer, 0, sizeof(char) * (len + 1));
    strncpy(buffer, color_yellow, yellow_len);
    strncpy(buffer + yellow_len, "Compiler: ", runtime_len);
    strncpy(buffer + yellow_len + runtime_len, color_reset, reset_len);
    strncpy(buffer + yellow_len + runtime_len + reset_len, format, format_len);
    buffer[len] = '\0';

    va_list args;
    va_start(args);
    vprintf(buffer, args);
    va_end(args);

    free(buffer);

    return false;
}

char *token_format(struct token_t *token);

static bool_t sym(struct token_t *token, const char *value)
{
    return token_equals(token, value, token_type_symbol);
}

static bool_t key(struct token_t *token, const char *value)
{
    return token_equals(token, value, token_type_keyword);
}

bool_t compiler_exec(comp_id id, const char *binary_path)
{
    if (0 == id) {
        return false;
    }

    printf("Creating compiler for target '%s'\n", binary_path);

    struct compiler_instance *instance = INST(id);

    char *asm_file = NULL;
    char *o_file = NULL;
    char *bin_file = NULL;
    FILE *f = NULL;
    bool_t success = false;

    usize_t len = strlen(binary_path);
    // Prepare asm_file path
    asm_file = (char *)malloc(sizeof(char) * (len + 4 + 1));
    if (!asm_file) {
        printf("Failed to create asm target path\n");
        goto _cleanup;
    }

    memset(asm_file, 0, sizeof(char) * (len + 4 + 1));
    strncpy(asm_file, binary_path, len);
    strncpy(asm_file + len, ".asm", strlen(".asm"));

    printf("Target assembly: %s\n", asm_file);

    // Prepare o_file path
    o_file = (char *)malloc(sizeof(char) * (len + 2 + 1));
    if (!o_file) {
        printf("Failed to create obj target path\n");
        goto _cleanup;
    }
    memset(o_file, 0, sizeof(char) * (len + 2 + 1));
    strncpy(o_file, binary_path, len);
    strncpy(o_file + len, ".o", strlen(".o"));

    printf("Target obj: %s\n", o_file);

    // Prepare bin_path
    bin_file = strdup(binary_path);
    if (!bin_file) {
        printf("Failed to create bin target path\n");
        goto _cleanup;
    }

    printf("Target bin: %s\n", bin_file);

    f = fopen(asm_file, "w");
    if (!f) {
        printf("Failed to open target asm file for writing\n");
        goto _cleanup;
    }

    printf("Writing ASM file\n");

    printf("Setting x86_64\n");
    // Init bits
    fprintf(f, "BITS 64\n");

    // Code section
    fprintf(f, "section .text\n");

    printf("Setting entry point\n");
    // Declare the global _start call
    fprintf(f, "\tglobal _start\n");

    printf("Generating helper calls\n");
    // Create a function for our `.` operator
    fprintf(f, "_log:\n");
    fprintf(f, "\tpush rbp\n");
    fprintf(f, "\tmov rbp, rsp\n");
    fprintf(f, "\tsub rsp, 32\n");
    fprintf(f, "\tlea rsi, [rbp-1]\n");
    fprintf(f, "\tmov byte [rsi], 10\n");
    fprintf(f, "\tmov rax, rdi\n");
    fprintf(f, "\ttest rax, rax\n");
    fprintf(f, "\tjz .handle_zero\n");
    fprintf(f, "\txor r8, r8\n");
    fprintf(f, "\ttest rax, rax\n");
    fprintf(f, "\tjns .convert_loop\n");
    fprintf(f, "\tneg rax\n");
    fprintf(f, "\tmov r8, 1\n");
    fprintf(f, ".convert_loop:\n");
    fprintf(f, "\txor rdx, rdx\n");
    fprintf(f, "\tmov rcx, 10\n");
    fprintf(f, "\tdiv rcx\n");
    fprintf(f, "\tadd dl, '0'\n");
    fprintf(f, "\tdec rsi\n");
    fprintf(f, "\tmov byte [rsi], dl\n");
    fprintf(f, "\ttest rax, rax\n");
    fprintf(f, "\tjnz .convert_loop\n");
    fprintf(f, "\ttest r8, r8\n");
    fprintf(f, "\tjz .print\n");
    fprintf(f, "\tdec rsi\n");
    fprintf(f, "\tmov byte [rsi], '-'\n");
    fprintf(f, "\tjmp .print\n");
    fprintf(f, ".handle_zero:\n");
    fprintf(f, "\tdec rsi\n");
    fprintf(f, "\tmov byte [rsi], '0'\n");
    fprintf(f, ".print:\n");
    fprintf(f, "\tlea rdx, [rbp-1]\n");
    fprintf(f, "\tsub rdx, rsi\n");
    fprintf(f, "\tinc rdx\n");
    fprintf(f, "\tmov rax, 1\n");
    fprintf(f, "\tmov rdi, 1\n");
    fprintf(f, "\tsyscall\n");
    fprintf(f, "\tmov rsp, rbp\n");
    fprintf(f, "\tpop rbp\n");
    fprintf(f, "\tret\n");

    printf("Generating _start\n");
    fprintf(f, "_start:\n");
    
    for (usize_t index = 0; index < instance->program_code->len; index++) {
        struct instruction_t *inst = vector_ptr_at(instance->program_code, index);

        struct token_t *t = inst->token;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, 255, "Generating instruction %s", t->value);

        size_t buf_len = strlen(buffer);

        //printf("\r Generating instruction %s", t->value);
        fprintf(f, "_lbl_inst_%lu:\t; %s", index, t->value);

        if (inst->next != -1) {
            snprintf(buffer + buf_len, 255 - buf_len, " (with next=%lu)", inst->next);
            buf_len = strlen(buffer);
            fprintf(f, " %lu", inst->next);
        }

        while (buf_len < strlen("Generating instruction XXXXX (with next=XXXXX)  ")) {
            buffer[buf_len++] = ' ';
        }

        snprintf(buffer + buf_len, 255 - buf_len, "%d%%            ",
                (int)((index + 1) * 100 / instance->program_code->len));

        printf("\r %s\r", buffer);
        fflush(stdout);
        fprintf(f, "\n");

        if (token_equals(t, NULL, token_type_number)) {
            usize_t num;
            token_get_number_value(t, &num);

            fprintf(f, "\tpush %lu\n", num);
        } else if (sym(t, ".")) {
            fprintf(f, "\tpop rdi\n");
            fprintf(f, "\tcall _log\n");
        } else if (sym(t, "+")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tadd rax, rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "-")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tsub rax, rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "*")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\timul rax, rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "/")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tdiv rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "%")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tdiv rbx\n");
            fprintf(f, "\tpush rdx\n");
        } else if (key(t, "dup")) {
            fprintf(f, "\tmov rax, [rsp]\n");
            fprintf(f, "\tpush rax\n");
        } else if (key(t, "over")) {
            fprintf(f, "\tmov rax, [rsp+8]\n");
            fprintf(f, "\tpush rax\n");
        } else if (key(t, "2dup")) {
            fprintf(f, "\tmov rax, [rsp+8]\n");
            fprintf(f, "\tmov rbx, [rsp]\n");
            fprintf(f, "\tpush rax\n");
            fprintf(f, "\tpush rbx\n");
        } else if (key(t, "swap")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpush rax\n");
            fprintf(f, "\tpush rbx\n");
        } else if (key(t, "2swap")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rcx\n");
            fprintf(f, "\tpop rdx\n");
            fprintf(f, "\tpush rbx\n");
            fprintf(f, "\tpush rax\n");
            fprintf(f, "\tpush rdx\n");
            fprintf(f, "\tpush rcx\n");
        } else if (key(t, "drop")) {
            fprintf(f, "\tadd rsp, 8\n");
        } else if (key(t, "if")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\ttest rax, rax\n");
            fprintf(f, "\tjz _lbl_inst_%lu\n", inst->next);
        } else if (key(t, "else")) {
            if (inst->next != -1) {
                fprintf(f, "\tjmp _lbl_inst_%lu\n", inst->next);
            }
        } else if (key(t, "end")) {
            if (inst->next != -1) {
                fprintf(f, "\tjmp _lbl_inst_%lu\n", inst->next);
            }
        } else if (sym(t, "=")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tcmp rax, rbx\n");
            fprintf(f, "\tsetz dl\n");
            fprintf(f, "\tpush rdx\n");
        } else if (sym(t, "!=")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tcmp rax, rbx\n");
            fprintf(f, "\tsetnz dl\n");
            fprintf(f, "\tpush rdx\n");
        } else if (sym(t, "<")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tcmp rax, rbx\n");
            fprintf(f, "\tsetl dl\n");
            fprintf(f, "\tpush rdx\n");
        } else if (sym(t, "<=")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tcmp rax, rbx\n");
            fprintf(f, "\tsetle dl\n");
            fprintf(f, "\tpush rdx\n");
        } else if (sym(t, ">")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tcmp rax, rbx\n");
            fprintf(f, "\tsetg dl\n");
            fprintf(f, "\tpush rdx\n");
        } else if (sym(t, ">=")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rdx, rdx\n");
            fprintf(f, "\tcmp rax, rbx\n");
            fprintf(f, "\tsetge dl\n");
            fprintf(f, "\tpush rdx\n");
        } else if (sym(t, "!")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rbx, rbx\n");
            fprintf(f, "\ttest rax, rax\n");
            fprintf(f, "\tsetz bl\n");
            fprintf(f, "\tpush rbx\n");
        } else if (sym(t, "~")) {
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tnot rax\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "&")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tand rax, rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "|")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\tor rax, rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "^")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rax, rbx\n");
            fprintf(f, "\tpush rax\n");
        } else if (sym(t, "&&")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rcx, rcx\n");
            fprintf(f, "\ttest rax, rax\n");
            fprintf(f, "\tjz .done\n");
            fprintf(f, "\ttest rbx, rbx\n");
            fprintf(f, "\tjz .done\n");
            fprintf(f, "\tmov rcx, 1\n");
            fprintf(f, ".done:\n");
            fprintf(f, "\tpush rcx\n");
        } else if (sym(t, "||")) {
            fprintf(f, "\tpop rbx\n");
            fprintf(f, "\tpop rax\n");
            fprintf(f, "\txor rcx, rcx\n");
            fprintf(f, "\ttest rax, rax\n");
            fprintf(f, "\tjnz .set_one\n");
            fprintf(f, "\ttest rbx, rbx\n");
            fprintf(f, "\tjz .done\n");
            fprintf(f, ".set_one:\n");
            fprintf(f, "\tmov rcx, 1\n");
            fprintf(f, ".done:\n");
            fprintf(f, "\tpush rcx\n");
        } else {
            char *token_log = token_format(t);
            printf("Invalid token [%s] at IP %lu\n", token_log, index);
            free(token_log);

            goto _cleanup;
        }
    }

    printf("\rGenerating instructions 100%%                            \r\n");

    printf("End of _start\n");

    fprintf(f, "_lbl_inst_%lu:\n_lbl_end:\n", instance->program_code->len);

    printf("Generating exit syscall\n");
    fprintf(f, "\tmov rax, 60\n");
    fprintf(f, "\tmov rdi, 0\n");
    fprintf(f, "\tsyscall\n");
    fprintf(f, "\tret ; this should not be reached\n");

    printf("Closing asm file\n");
    fflush(f);
    fclose(f);

    printf("Assembling\n");

    char *nasm_args[] = {
        "-f",
        "elf64",
        asm_file,
        NULL
    };

    if (!util_command_ok("nasm", nasm_args)) {
        printf("Failed to assemble binary\n");
        goto _cleanup;
    }

    printf("Linking\n");

    char *ld_args[] = {
        "-o",
        bin_file,
        o_file,
        NULL
    };

    if (util_file_exist("./.__tmp")) {
        util_file_delete("./.__tmp");
    }

    if (util_file_exist(bin_file)) {
        util_file_move(bin_file, "./.__tmp");
    }

    if (!util_command_ok("ld", ld_args)) {
        printf("Failed to link bindary\n");

        if (util_file_exist("./.__tmp")) {
            util_file_move("./.__tmp", bin_file);
        }

        goto _cleanup;
    }

    printf("Finalizing\n");

    if (util_file_exist("./.__tmp")) {
        util_file_delete("./.__tmp");
    }

    if (util_file_exist(o_file)) {
        util_file_delete(o_file);
    }

    printf("--> Output Binary: %s\n", bin_file);

    success = true;

_cleanup:
    if (asm_file) {
        free(asm_file);
    }

    if (o_file) {
        free(o_file);
    }

    if (bin_file) {
        free(bin_file);
    }

    return success;
}

bool_t compiler_release(comp_id id)
{
    if (0 == id) {
        return false;
    }

    struct compiler_instance *instance = INST(id);

    instruction_free_vector(instance->program_code);
    free(instance->program_code);
    instance->program_code = NULL;

    free(instance);

    return true;
}
