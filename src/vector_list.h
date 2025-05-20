#ifndef _CSTACK_VECTOR_LIST_H
#define _CSTACK_VECTOR_LIST_H

#include "type.h"

struct vector_num_t {
    usize_t num;
    usize_t den;
};

struct vector_t {
    usize_t capacity;
    usize_t len;
    usize_t *data;
    struct vector_num_t inc;
};

bool_t vector_alloc(struct vector_t *vec, usize_t capacity);
bool_t vector_release(struct vector_t *vec);
bool_t vector_includes(struct vector_t *vec, usize_t item);
bool_t vector_add(struct vector_t *vec, usize_t item);
bool_t vector_erase_at(struct vector_t *vec, usize_t index);
bool_t vector_erase_all(struct vector_t *vec, usize_t item);
bool_t vector_find(struct vector_t *vec, usize_t item, usize_t *index);
bool_t vector_clear(struct vector_t *vec);
bool_t vector_at(struct vector_t *vec, usize_t index, usize_t *item);
void * vector_ptr_at(struct vector_t *vec, usize_t index);
 
#endif //_CSTACK_VECTOR_LIST_H
