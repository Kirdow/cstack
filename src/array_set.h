#ifndef _CSTACK_ARRAY_SET_H
#define _CSTACK_ARRAY_SET_H

#include "type.h"

struct array_set_t {
    usize_t capacity;
    usize_t len;
    usize_t *data;
};

bool_t array_set_alloc(struct array_set_t *set, usize_t capacity);
bool_t array_set_release(struct array_set_t *set);
bool_t array_set_includes(struct array_set_t *set, usize_t item);
bool_t array_set_add(struct array_set_t *set, usize_t item); 
bool_t array_set_erase(struct array_set_t *set, usize_t item);
bool_t array_set_clear(struct array_set_t *set);

bool_t _array_set_dump(struct array_set_t *set);


#endif // _CSTACK_ARRAY_SET_H
