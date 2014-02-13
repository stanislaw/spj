

#ifndef spj_jsonvalue_h
#define spj_jsonvalue_h


#include "types.h"


void spj_jsonvalue_init(spj_jsonvalue_t *jsonvalue, spj_jsonvalue_type_t type);
void spj_jsonvalue_free(spj_jsonvalue_t *jsonvalue);


#endif

