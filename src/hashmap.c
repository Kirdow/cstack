#include "hashmap.h"

#include <stdlib.h>
#include <string.h>

struct hashmap_t * hashmap_create(void)
{
    struct hashmap_t *hashmap = (struct hashmap_t *)malloc(sizeof(struct hashmap_t));

    hashmap->size = 32;
    hashmap->buckets = (struct hashmap_entry_t **)malloc(sizeof(struct hashmap_entry_t *) * hashmap->size);
    memset(hashmap->buckets, 0, sizeof(struct hashmap_entry_t *) * hashmap->size);

    return hashmap;
}

static usize_t hash(const char *key)
{
    usize_t hash = 5381;
    ssize_t c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

bool_t hashmap_put(struct hashmap_t *hashmap, const char *key, const char *value)
{
    if (NULL == hashmap) {
        return false;
    }

    usize_t index = hash(key) % hashmap->size;
    struct hashmap_entry_t *current = hashmap->buckets[index];
    
    while (NULL != current) {
        if (strcmp(current->key, key) == 0) {
            free(current->value);
            current->value = strdup(value);
            return true;
        }

        current = current->next;
    }

    struct hashmap_entry_t *new_entry = (struct hashmap_entry_t *)malloc(sizeof(struct hashmap_entry_t));
    if (NULL == new_entry) {
        return false;
    }

    new_entry->key = strdup(key);
    new_entry->value = strdup(value);

    new_entry->next = hashmap->buckets[index];
    hashmap->buckets[index] = new_entry;

    return true;
}

const char * hashmap_get(struct hashmap_t *hashmap, const char *key)
{
    if (NULL == hashmap) {
        return NULL;
    }

    usize_t index = hash(key) % hashmap->size;
    struct hashmap_entry_t *current = hashmap->buckets[index];

    while (NULL != current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }

        current = current->next;
    }

    return NULL;
}

bool_t hashmap_remove(struct hashmap_t *hashmap, const char *key)
{
    if (NULL == hashmap) {
        return false;
    }

    usize_t index = hash(key) % hashmap->size;
    struct hashmap_entry_t *current = hashmap->buckets[index];
    struct hashmap_entry_t *prev = NULL;

    while (NULL != current) {
        if (strcmp(current->key, key) == 0) {
            if (NULL == prev) {
                hashmap->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }

            free(current->key);
            free(current->value);
            free(current);
            return true;
        }

        prev = current;
        current = current->next;
    }

    return false;
}

bool_t hashmap_destroy(struct hashmap_t *hashmap)
{
    if (NULL == hashmap) {
        return true;
    }

    if (NULL != hashmap->buckets) {
        for (usize_t index = 0; index < hashmap->size; index++) {
            struct hashmap_entry_t *current = hashmap->buckets[index];

            while (NULL != current) {
                struct hashmap_entry_t *next = current->next;
                free(current->key);
                free(current->value);
                free(current);
                current = next;
            }
        }

        free(hashmap->buckets);
        hashmap->buckets = NULL;
    }

    free(hashmap);

    return true;
}
