

#ifndef spj_jsonvalue_h
#define spj_jsonvalue_h


#include "types.h"


void spj_jsonvalue_init(SpjJSONValue *jsonvalue, spj_jsonvalue_type_t type);

void spj_jsonvalue_object_add(SpjJSONValue *jsonvalue, SpjJSONNamedValue *child_jsonvalue, size_t *capacity);
void spj_jsonvalue_array_add(SpjJSONValue *jsonvalue, SpjJSONValue *child_jsonvalue, size_t *capacity);

void spj_jsonvalue_object_finalize(SpjJSONValue *jsonvalue, size_t *capacity);
void spj_jsonvalue_array_finalize(SpjJSONValue *jsonvalue, size_t *capacity);


void spj_jsonvalue_enumerate(SpjJSONValue *jsonvalue, int (*function)(SpjJSONValue *jsonvalue));
void spj_jsonvalue_enumerate_reverse(SpjJSONValue *jsonvalue, int (*function)(SpjJSONValue *jsonvalue));

void spj_jsonvalue_free(SpjJSONValue *jsonvalue);


#endif

