

#ifndef spj_jsonvalue_h
#define spj_jsonvalue_h


#include "types.h"


void spj_jsonvalue_init(spj_jsonvalue_t *jsonvalue, spj_jsonvalue_type_t type);

void spj_jsonvalue_object_add(spj_jsonvalue_t *jsonvalue, spj_jsonnamedvalue_t *child_jsonvalue, size_t *capacity);
void spj_jsonvalue_array_add(spj_jsonvalue_t *jsonvalue, spj_jsonvalue_t *child_jsonvalue, size_t *capacity);

void spj_jsonvalue_object_finalize(spj_jsonvalue_t *jsonvalue, size_t *capacity);
void spj_jsonvalue_array_finalize(spj_jsonvalue_t *jsonvalue, size_t *capacity);


void spj_jsonvalue_enumerate(spj_jsonvalue_t *jsonvalue, int (*function)(spj_jsonvalue_t *jsonvalue));
void spj_jsonvalue_enumerate_reverse(spj_jsonvalue_t *jsonvalue, int (*function)(spj_jsonvalue_t *jsonvalue));

void spj_jsonvalue_free(spj_jsonvalue_t *jsonvalue);


#endif

