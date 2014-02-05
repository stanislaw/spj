
#ifndef spj_h
#define spj_h


#include "types.h"
#include "debug.h"


spj_result_t spj_parse(const char *jsonstring, SpjJSONValue *jsonvalue, spj_error_t *error);
int spj_delete (SpjJSONValue *jsonvalue);


#endif




