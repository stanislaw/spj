
#ifndef spj_h
#define spj_h


#include "types.h"
#include "jsonvalue.h"
#include "debug.h"


spj_result_t spj_parse(const char *jsonstring, size_t datasize, SpjJSONValue *jsonvalue, spj_error_t *error);
void spj_jsonvalue_free(SpjJSONValue *jsonvalue);


#endif

