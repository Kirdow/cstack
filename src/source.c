#include "source.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool_t source_read_file(const char *path, char **data)
{
    char *buffer = NULL;
    usize_t len;

    FILE *f = fopen(path, "rb");

    if (NULL == f) {
        return false;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = (char *)malloc(sizeof(char) * (len + 1));
    memset(buffer, 0, sizeof(char) * (len + 1));
    if (NULL != buffer) {
        fread(buffer, 1, len, f);
    }
    fclose(f);

    if (NULL != buffer) {
        if (NULL != data) {
            *data = buffer;
        }

        return true;
    }

    return false;
}

bool_t source_free_file(char **ptr)
{
    if (NULL == ptr) {
        return false;
    }

    char *data = *ptr;

    if (NULL == data) {
        return false;
    }

    free(data);
    *ptr = NULL;

    return true;
}
