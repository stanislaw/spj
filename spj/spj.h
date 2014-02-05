
#ifndef spj_h
#define spj_h


#include "types.h"
#include "jsonvalue.h"
#include "debug.h"

// Public API

spj_result_t spj_parse(const char *jsonstring, SpjJSONValue *jsonvalue, spj_error_t *error);
extern int spj_jsonvalue_free(SpjJSONValue *jsonvalue);


#endif

