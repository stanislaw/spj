

#ifndef spj_jsonvalue_h
#define spj_jsonvalue_h

#import "types.h"

void spj_jsonvalue_init(SpjJSONValue *jsonvalue, SpjJSONValueType type);

void spj_jsonvalue_object_add(SpjJSONValue *jsonvalue, SpjJSONNamedValue *child_jsonvalue, size_t *capacity);
void spj_jsonvalue_array_add(SpjJSONValue *jsonvalue, SpjJSONValue *child_jsonvalue, size_t *capacity);

void spj_jsonvalue_object_finalize(SpjJSONValue *jsonvalue, size_t *capacity);
void spj_jsonvalue_array_finalize(SpjJSONValue *jsonvalue, size_t *capacity);

#endif