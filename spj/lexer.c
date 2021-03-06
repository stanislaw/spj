

#include "lexer.h"
#include "debug.h"
#include "jsonvalue.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


spj_lexer_t spj_lexer_create(const char *jsonbytes, size_t datasize) {
    spj_lexer_t lexer;

    lexer.data = jsonbytes;
    lexer.currentposition = 0;
    lexer.datasize = datasize;
    lexer.error = NULL;

    lexer.buf_used = 0;
    lexer.buf_capacity = 32;
    lexer.buf_keys = malloc(lexer.buf_capacity * sizeof(spj_string_t));
    lexer.buf_values = malloc(lexer.buf_capacity * sizeof(spj_jsonvalue_t));
    
    return lexer;
}


void spj_lexer_free(spj_lexer_t *lexer) {
    free(lexer->buf_keys);
    free(lexer->buf_values);
}


void spj_lexer_increment_buf_used(spj_lexer_t *lexer) {
    lexer->buf_used++;

    if (lexer->buf_used == lexer->buf_capacity) {
        lexer->buf_capacity = lexer->buf_capacity << 1;

        spj_string_t *larger_keys = realloc(lexer->buf_keys, lexer->buf_capacity * sizeof(spj_string_t));
        spj_jsonvalue_t *larger_values = realloc(lexer->buf_values, lexer->buf_capacity * sizeof(spj_jsonvalue_t));

        if (larger_keys != NULL && larger_values != NULL) {
            lexer->buf_keys = larger_keys;
            lexer->buf_values = larger_values;
        } else {
            assert(0); /* Handle out of memory */
        }
    }
}



void spj_lexer_increment(spj_lexer_t *lexer) {
    lexer->currentposition++;

    /* Check datasize = 0 case later */
    if (lexer->currentposition == (lexer->datasize + 1) && lexer->datasize > 0) {
        print_callstack();
        assert(0);
    }
}


char spj_lexer_peek(spj_lexer_t *lexer) {
    return lexer->data[lexer->currentposition];
}


char spj_lexer_getc(spj_lexer_t *lexer) {
    char c = lexer->data[lexer->currentposition];

    spj_lexer_increment(lexer);

    return c;
}


const char * spj_lexer_getp(spj_lexer_t *lexer) {
    const char *p = lexer->data + lexer->currentposition;

    spj_lexer_increment(lexer);

    return p;
}


const char * spj_lexer_peekp(spj_lexer_t *lexer) {
    return lexer->data + lexer->currentposition;
}


static size_t spj_lexer_seek(spj_lexer_t *lexer, int offset) {
    if (offset > 0) {
        lexer->currentposition += offset;

        if (lexer->currentposition > lexer->datasize - 1) {
            lexer->currentposition = lexer->datasize - 1;
        }
    } else if (offset < 0) {
        unsigned int uoffset = (unsigned int)abs(offset);

        if (lexer->currentposition > uoffset) {
            lexer->currentposition -= uoffset;
        } else {
            lexer->currentposition = 0;
        }
    }

    return lexer->currentposition;
}


#pragma mark
#pragma mark Lexer functions


spj_jsontoken_type_t spj_gettoken_string(spj_lexer_t *lexer) {
    spj_jsonvalue_t jsonvalue;
    spj_string_t string;
    size_t firstchar_position;
    const char *firstchar_ptr;

    assert(lexer->data[lexer->currentposition - 1] == '"');

    firstchar_position = lexer->currentposition;

    while (spj_lexer_peek(lexer) != '"') {
        spj_lexer_increment(lexer);
    }

    string.size = lexer->currentposition - firstchar_position;
    string.data = malloc((string.size + 1) * sizeof(char));

    firstchar_ptr = lexer->data + firstchar_position;

    memcpy(string.data, firstchar_ptr, string.size);
    string.data[string.size] = '\0';

    /* printf("Token[String] : %s(%lu, %lu)\n", string.data, strlen(string.data), string.size); */

    spj_jsonvalue_init(&jsonvalue, SpjValueString);

    jsonvalue.value.string = string;

    lexer->value = jsonvalue;

    assert(spj_lexer_peek(lexer) == '"');

    spj_lexer_increment(lexer);

    return SpjTokenString;
}


spj_jsontoken_type_t spj_gettoken_number(spj_lexer_t *lexer) {
    spj_jsonvalue_t jsonvalue;
    char currentbyte;
    const char *firstbyte;
    char *endpointer = NULL;
    int err;

    firstbyte = spj_lexer_getp(lexer);

    spj_jsonvalue_init(&jsonvalue, SpjValueNumber);

    currentbyte = spj_lexer_peek(lexer);

    if ((isdigit(*firstbyte) || *firstbyte == '-') == 0) {
        assert(0);
    }

    jsonvalue.value.number = strtod(firstbyte, &endpointer);

    err = errno;
    if (jsonvalue.value.number == 0 && firstbyte == endpointer) {
        assert(0);
    }

    if (err == ERANGE) {
        assert(0);
    }

    while (spj_lexer_peekp(lexer) < endpointer) {
        spj_lexer_increment(lexer);
    }

    lexer->value = jsonvalue;

    /* printf("Token[Number] : %f\n", jsonvalue.value.number); */

    assert(isdigit(spj_lexer_peek(lexer)) == 0);

    return SpjTokenNumber;
}


spj_jsontoken_type_t spj_gettoken(spj_lexer_t *lexer) {
    char currentbyte;

    while (isspace(currentbyte = spj_lexer_getc(lexer)));

    switch (currentbyte) {
        case '\0':
            return SpjTokenEOS;

        case '[':
            return SpjTokenArrayStart;

        case ']':
            return SpjTokenArrayEnd;

        case '{':
            return SpjTokenObjectStart;
        
        case '}':
            return SpjTokenObjectEnd;

        case ':':
            return SpjTokenColon;

        case ',':
            return SpjTokenComma;

        case '"':
            return spj_gettoken_string(lexer);
    }

    if (isdigit(currentbyte) || currentbyte == '-') {
        spj_lexer_seek(lexer, -1);

        return spj_gettoken_number(lexer);
    }

    if (currentbyte == 't') {
        if (spj_lexer_getc(lexer) == 'r' &&
            spj_lexer_getc(lexer) == 'u' &&
            spj_lexer_getc(lexer) == 'e') {

            /* printf("Token[Bool] : true\n"); */

            spj_jsonvalue_t jsonvalue;
            spj_jsonvalue_init(&jsonvalue, SpjValueTrue);

            lexer->value = jsonvalue;

            return SpjTokenTrue;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'f') {
        if (spj_lexer_getc(lexer) == 'a' &&
            spj_lexer_getc(lexer) == 'l' &&
            spj_lexer_getc(lexer) == 's' &&
            spj_lexer_getc(lexer) == 'e') {

            /* printf("Token[Bool] : false\n"); */

            spj_jsonvalue_t jsonvalue;
            spj_jsonvalue_init(&jsonvalue, SpjValueFalse);

            lexer->value = jsonvalue;

            return SpjTokenFalse;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'n') {
        if (spj_lexer_getc(lexer) == 'u' &&
            spj_lexer_getc(lexer) == 'l' &&
            spj_lexer_getc(lexer) == 'l') {
            
            /* printf("Token[Null]\n"); */

            spj_jsonvalue_t jsonvalue;
            spj_jsonvalue_init(&jsonvalue, SpjValueNull);

            lexer->value = jsonvalue;
            
            return SpjTokenNull;
            
        } else {
            assert(0);
        }
    }
    
    
    /* We should not reach here */
    
    assert(0);
    
    return SpjTokenError;
}

