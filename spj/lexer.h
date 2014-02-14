
#ifndef spj_lexer_h
#define spj_lexer_h

#include <stdio.h>
#include "types.h"


typedef enum spj_jsontoken_type_t {
    SpjJSONTokenError =       0,
    SpjJSONTokenNumber =      1,
    SpjJSONTokenString =      2,
    SpjJSONTokenNull =        3,
    SpjJSONTokenTrue =        4,
    SpjJSONTokenFalse =       5,
    SpjJSONTokenObjectStart = 6,
    SpjJSONTokenArrayStart =  7,
    SpjJSONTokenObjectEnd =   8,
    SpjJSONTokenArrayEnd =    9,
    SpjJSONTokenColon =      10,
    SpjJSONTokenComma =      11,
    SpjJSONTokenEOS =        12
} spj_jsontoken_type_t;


typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;

    spj_jsonvalue_t value;

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
