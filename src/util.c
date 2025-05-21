#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 4096

static char **dup_args_with_command(const char *command, char **args)
{
    size_t count = 1;
    for (size_t i = 0; args[i] != NULL; i++) {
        ++count;
    }

    char **dup = (char **)malloc(sizeof(char *) * (count + 1));

    dup[0] = strdup(command);

    for (size_t i = 0; args[i] != NULL; i++) {
        dup[1 + i] = strdup(args[i]);
    }

    dup[count] = NULL;

    return dup;
}

static void dup_free(char **args)
{
    for (size_t i = 0; args[i] != NULL; i++) {
        char *arg = args[i];
        free(arg);
    }

    free(args);
}

static void print_command(const char *command, const char **args)
{
    printf("%s", command);
    for (int i = 0; args[i] != NULL; i++) {
        printf(" %s", args[i]);
    }
}

int util_command(const char *command, char **args, char *output, size_t output_size, int *exit_code)
{
    printf("$ ");
    print_command(command, (const char **)args);
    printf("\n");
    fflush(stdout);

    args = dup_args_with_command(command, args);


    int pipefd[2];
    pid_t pid;
    int status;
    ssize_t bytes_read;
    size_t total_bytes = 0;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        dup_free(args);
        return -1;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        dup_free(args);
        return -1;
    }

    if (pid == 0) {
        close(pipefd[0]);

        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        if (dup2(pipefd[1], STDERR_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]);

        execvp(command, args);

        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);

        output[0] = '\0';
        while ((bytes_read = read(pipefd[0], output + total_bytes,
                        output_size - total_bytes - 1)) > 0) {
            total_bytes += bytes_read;
            if (total_bytes >= output_size - 1) {
                break;
            }
        }

        output[total_bytes] = '\0';

        close(pipefd[0]);

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            dup_free(args);
            return -1;
        }

        if (WIFEXITED(status)) {
            *exit_code = WEXITSTATUS(status);
        } else {
            *exit_code = 1;
        }

        dup_free(args);
        return 0;
    }
}

bool_t util_command_ok(const char *command, char **args)
{
    char output[BUFFER_SIZE];
    int exit_code;
    if (util_command(command, args, output, sizeof(output), &exit_code) == 0) {
        if (strlen(output) != 0) {
            printf("%s\n", output);
        }

        if (exit_code == 0) {
            return true;
        }


        printf("Executed command returned code %d.\n", exit_code);
        return false;
    } else {
        printf("Command execution failed.\n");
        return false;
    }
}

bool_t util_file_exist(const char *path)
{
    return access(path, F_OK) == 0;
}

bool_t util_file_delete(const char *path)
{
    return remove(path) == 0;
}

bool_t util_file_move(const char *path, const char *dest)
{
    return rename(path, dest) == 0;
}
