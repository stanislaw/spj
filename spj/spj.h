
#ifndef spj_h
#define spj_h


#include "types.h"
#include "jsonvalue.h"
#include "debug.h"


spj_result_t spj_parse(const char *jsonstring, size_t datasize, spj_jsonvalue_t *jsonvalue, spj_error_t *error);
void spj_jsonvalue_free(spj_jsonvalue_t *jsonvalue);


#endif

