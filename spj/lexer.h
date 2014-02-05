
#ifndef spj_lexer_h
#define spj_lexer_h

#include <stdio.h>
#include "types.h"


typedef enum SpjJSONTokenType {
    SpjJSONTokenError =       0,
    SpjJSONTokenNumber =      1,
    SpjJSONTokenString =      2,
    SpjJSONTokenBool =        3, // true false
    SpjJSONTokenNull =        4, // null
    SpjJSONTokenObjectStart = 5, // {
    SpjJSONTokenArrayStart =  6, // [
    SpjJSONTokenObjectEnd =   7, // }
    SpjJSONTokenArrayEnd =    8, // ]
    SpjJSONTokenColon =       9, // : =
    SpjJSONTokenComma =      10, // ,
    SpjJSONTokenEOS =        11 //
} SpjJSONTokenType;


typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;
} spj_iterator_t;


typedef struct {
    spj_iterator_t *iterator;
    SpjJSONValue value;
    spj_error_t *error;
} spj_lexer_t;


char spj_iterator_peek(spj_iterator_t *iterator);
spj_iterator_t spj_iterator_create(const char *jsonbytes, size_t datasize);


SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer);
SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer);
SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer);



#endif
