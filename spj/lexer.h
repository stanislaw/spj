
#ifndef spj_lexer_h
#define spj_lexer_h

#include <stdio.h>
#include "types.h"


typedef enum spj_jsontoken_type_t {
    SpjJSONTokenError =       0,
    SpjJSONTokenNumber =      1,
    SpjJSONTokenString =      2,
    SpjJSONTokenBool =        3,
    SpjJSONTokenNull =        4,
    SpjJSONTokenObjectStart = 5,
    SpjJSONTokenArrayStart =  6,
    SpjJSONTokenObjectEnd =   7,
    SpjJSONTokenArrayEnd =    8,
    SpjJSONTokenColon =       9,
    SpjJSONTokenComma =      10,
    SpjJSONTokenEOS =        11 
} spj_jsontoken_type_t;


typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;

    SpjJSONValue value;

    SpjJSONValue *shared_array_elements;
    SpjJSONNamedValue *shared_object_elements;


    spj_error_t *error;
} spj_lexer_t;


char spj_lexer_peek(spj_lexer_t *lexer);
spj_lexer_t spj_lexer_create(const char *jsonbytes, size_t datasize);
void spj_lexer_increment(spj_lexer_t *lexer);
char spj_lexer_getc(spj_lexer_t *lexer);
const char * spj_lexer_getp(spj_lexer_t *lexer);
const char * spj_lexer_peekp(spj_lexer_t *lexer);


spj_jsontoken_type_t spj_gettoken_string(spj_lexer_t *lexer);
spj_jsontoken_type_t spj_gettoken_number(spj_lexer_t *lexer);
spj_jsontoken_type_t spj_gettoken(spj_lexer_t *lexer);



#endif
