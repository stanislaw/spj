
#ifndef spj_lexer_h
#define spj_lexer_h

#include <stdio.h>
#include "types.h"


typedef enum spj_jsontoken_type_t {
    SpjTokenError =       0,
    SpjTokenNumber =      1,
    SpjTokenString =      2,
    SpjTokenNull =        3,
    SpjTokenTrue =        4,
    SpjTokenFalse =       5,
    SpjTokenObjectStart = 6,
    SpjTokenArrayStart =  7,
    SpjTokenObjectEnd =   8,
    SpjTokenArrayEnd =    9,
    SpjTokenColon =      10,
    SpjTokenComma =      11,
    SpjTokenEOS =        12
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
