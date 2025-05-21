#ifndef _CSTACK_UTIL_H
#define _CSTACK_UTIL_H

#include "type.h"

#include <stddef.h>

int util_command(const char *command, char **args, char *output, size_t output_size, int *exit_code);
bool_t util_command_ok(const char *command, char **args);

bool_t util_file_exist(const char *path);
bool_t util_file_delete(const char *path);
bool_t util_file_move(const char *path, const char *dest);

#endif
