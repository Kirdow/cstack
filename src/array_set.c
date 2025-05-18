#include "array_set.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool_t array_set_alloc(struct array_set_t *set, usize_t capacity)
{
    if (NULL == set) {
        return false;
    }

    usize_t *data = NULL;

    data = malloc(capacity * sizeof(usize_t));
    if (NULL == data) {
        return false;
    }

    memset(data, 0, sizeof(usize_t) * capacity);
    set->capacity = capacity;
    set->len = 0;
    set->data = data;

    return true;
}

bool_t array_set_release(struct array_set_t *set)
{
    if (NULL == set) {
        return false;
    }

    if (NULL != set->data) {
        free(set->data);
        set->data = NULL;
    }

    set->capacity = 0;
    set->len = 0;

    return true;
}

bool_t array_set_includes(struct array_set_t *set, usize_t item)
{
    if (NULL == set || NULL == set->data) {
        return false;
    }

    for (usize_t i = 0; i < set->len; i++) {
        if (item == set->data[i]) {
            return true;
        }
    }

    return false;
}

bool_t array_set_add(struct array_set_t *set, usize_t item)
{
    if (NULL == set || NULL == set->data) {
        return false;
    }

    for (usize_t i = 0; i < set->len; i++) {
        if (item == set->data[i]) {
            return true;
        }
    }

    if (set->len >= set->capacity) {
        return false;
    }

    set->data[set->len] = item;
    set->len += 1;

    return true;
}

bool_t array_set_erase(struct array_set_t *set, usize_t item)
{
    if (NULL == set || NULL == set->data) {
        return false;
    }

    for (usize_t i = 0; i < set->len; i++) {
        if (item == set->data[i]) {
            if (i + 1 < set->len) {
                // Move last item into this place
                set->data[i] = set->data[set->len - 1];
            }

            set->len -= 1;

            return true;
        }
    }

    return true;
}

bool_t array_set_clear(struct array_set_t *set)
{
    if (NULL == set || NULL == set->data) {
        return false;
    }

    memset(set->data, 0, sizeof(usize_t) * set->capacity);
    set->len = 0;

    return true;
}

bool_t _array_set_dump(struct array_set_t *set)
{
    if (NULL == set || NULL == set->data) {
        return false;
    }

    printf("Cap %lu\n", set->capacity);
    printf("Len %lu\n", set->len);
    for (usize_t i = 0; i < set->len; i++) {
        printf(" >%lu\t: %lu\n", i, set->data[i]);
    }
}
