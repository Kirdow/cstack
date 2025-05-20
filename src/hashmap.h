#ifndef _CSTACK_HASHMAP_H
#define _CSTACK_HASHMAP_H

#include "type.h"

struct hashmap_entry_t {
    char *key;
    char *value;
    struct hashmap_entry_t *next;
};

struct hashmap_t {
    struct hashmap_entry_t **buckets;
    usize_t size;
};

struct hashmap_t * hashmap_create(void);
bool_t hashmap_destroy(struct hashmap_t *hashmap);
bool_t hashmap_put(struct hashmap_t *hashmap, const char *key, const char *value);
const char * hashmap_get(struct hashmap_t *hashmap, const char *key);
bool_t hashmap_remove(struct hashmap_t *hashmap, const char *key);

#endif //_CSTACK_HASHMAP_H
