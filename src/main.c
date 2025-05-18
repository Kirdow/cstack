#include <stdio.h>

#include "array_set.h"

int main(int argc, char** argv)
{
    printf("-Creating set of capacity 8\n");
    struct array_set_t set;
    array_set_alloc(&set, 8);

    _array_set_dump(&set);

    printf("-Adding 6 19 7 and 3\n");
    array_set_add(&set, 6);
    array_set_add(&set, 19);
    array_set_add(&set, 7);
    array_set_add(&set, 3);

    _array_set_dump(&set);

    printf("-Remove 19 and add 17\n");
    array_set_erase(&set, 19);
    array_set_add(&set, 17);

    _array_set_dump(&set);

    printf("-Clear set\n");
    array_set_clear(&set);

    _array_set_dump(&set);

    printf("-Release memory\n");
    array_set_release(&set);

    _array_set_dump(&set);

    printf("-Done\n");

    return 0;
}
