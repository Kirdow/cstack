#include "vector_list.h"

#include <stdlib.h>
#include <string.h>

bool_t vector_alloc(struct vector_t *vec, usize_t capacity)
{
    if (NULL == vec) {
        return false;
    }

    vec->capacity = capacity < 1 ? 1 : capacity;
    vec->len = 0;
    vec->inc.num = 3;
    vec->inc.den = 2;

    vec->data = malloc(sizeof(usize_t) * vec->capacity);
    if (NULL == vec->data) {
        memset(vec, 0, sizeof(struct vector_t));

        return false;
    }

    memset(vec->data, 0, sizeof(usize_t) * vec->capacity);
    return true;
}

bool_t vector_release(struct vector_t *vec)
{
    if (NULL == vec) {
        return false;
    }

    if (NULL != vec->data) {
        free(vec->data);
        vec->data = NULL;
    }

    memset(vec, 0, sizeof(struct vector_t));
    return true;
}

bool_t vector_includes(struct vector_t *vec, usize_t item)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    for (usize_t i = 0; i < vec->len; i++) {
        if (vec->data[i] == item) {
            return true;
        }
    }

    return false;
}

bool_t vector_add(struct vector_t *vec, usize_t item)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    if (vec->len >= vec->capacity) {
        vec->len = vec->capacity;

        usize_t new_cap = vec->capacity * vec->inc.num / vec->inc.den;
        if (new_cap <= vec->capacity) {
            new_cap = vec->capacity + 1;
        }

        vec->data = realloc(vec->data, sizeof(usize_t) * new_cap);
        memset(&vec->data[vec->capacity], 0, sizeof(usize_t) * (new_cap - vec->capacity));

        vec->capacity = new_cap;
    }

    vec->data[vec->len++] = item;
    return true;
}

bool_t vector_erase_at(struct vector_t *vec, usize_t index)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    if (index >= vec->len) {
        return false;
    }

    for (usize_t i = index; i < vec->len - 1; i++) {
        vec->data[i] = vec->data[i + 1];
        vec->data[i + 1] = 0; // just to remove any old data
    }

    --vec->len;

    return true;
}

bool_t vector_erase_all(struct vector_t *vec, usize_t item)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    usize_t index;
    while (vector_find(vec, item, &index)) {
        vector_erase_at(vec, index);
    }

    return true;
}

bool_t vector_find(struct vector_t *vec, usize_t item, usize_t *index)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    for (usize_t i = 0; i < vec->len; i++) {
        if (vec->data[i] == item) {
            if (NULL != index) {
                *index = i;
            }

            return true;
        }
    }

    return false;
}

bool_t vector_at(struct vector_t *vec, usize_t index, usize_t *item)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    if (index >= vec->len) {
        return false;
    }

    if (NULL != item) {
        *item = vec->data[index];
    }

    return true;
}

void * vector_ptr_at(struct vector_t *vec, usize_t index)
{
    void *item;
    if (!vector_at(vec, index, (usize_t *)&item)) {
        return NULL;
    }

    return item;
}

bool_t vector_clear(struct vector_t *vec)
{
    if (NULL == vec || NULL == vec->data) {
        return false;
    }

    memset(vec->data, 0, sizeof(usize_t) * vec->capacity);
    vec->len = 0;

    return true;
}


