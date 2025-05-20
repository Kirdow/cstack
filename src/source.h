#ifndef _CSTACK_SOURCE_H
#define _CSTACK_SOURCE_H

#include "type.h"

bool_t source_read_file(const char *path, char **data);
bool_t source_free_file(char **ptr);


#endif //_CSTACK_SOURCE_H
