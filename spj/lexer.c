

#include "lexer.h"
#include "debug.h"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>


/*
    SpjJSONValueType spj_value_for_token(SpjJSONTokenType token) {
        switch (token) {
            case SpjJSONTokenObjectStart: {
                return SpjJSONValueObject;
            }

            case SpjJSONTokenArrayStart: {
                return SpjJSONValueArray;
            }

            case SpjJSONTokenNumber: {
                return SpjJSONValueNumber;
            }

            case SpjJSONTokenString: {
                return SpjJSONValueString;
            }

            case SpjJSONTokenBool: {
                return SpjJSONValueBool;
            }

            case SpjJSONTokenNull: {
                return SpjJSONValueNull;
            }

            default:
                assert(0);
        }
        
        return 0;
    }
*/


spj_lexer_t spj_lexer_create(const char *jsonbytes, size_t datasize) {
    spj_lexer_t lexer;

    lexer.data = jsonbytes;
    lexer.currentposition = 0;
    lexer.datasize = datasize;
    lexer.error = NULL;

    return lexer;
}


static void spj_lexer_increment(spj_lexer_t *lexer) {
    lexer->currentposition++;

    // Check datasize = 0 case later
    if (lexer->currentposition == (lexer->datasize + 1) && lexer->datasize > 0) {
        print_callstack();
        assert(0);
    }
}


char spj_lexer_peek(spj_lexer_t *lexer) {
    return lexer->data[lexer->currentposition];
}


static char spj_lexer_getc(spj_lexer_t *lexer) {
    char c = lexer->data[lexer->currentposition];

    spj_lexer_increment(lexer);

    return c;
}


static size_t spj_lexer_seek(spj_lexer_t *lexer, int offset) {
    if (offset > 0) {
        lexer->currentposition += offset;

        if (lexer->currentposition > lexer->datasize - 1) {
            lexer->currentposition = lexer->datasize - 1;
        }
    } else if (offset < 0) {
        unsigned int uoffset = (unsigned int)(-offset);

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


SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer) {
    SpjString string;

    assert(lexer->data[lexer->currentposition - 1] == '"');

    size_t firstchar_position = lexer->currentposition;

    while (spj_lexer_peek(lexer) != '"') {
        spj_lexer_increment(lexer);
    }

    string.size = lexer->currentposition - firstchar_position;
    string.data = (char *)malloc((string.size + 1) * sizeof(char)); // memento

    const char *firstchar_ptr = lexer->data + firstchar_position;

    memcpy(string.data, firstchar_ptr, string.size);
    string.data[string.size] = '\0';

    printf("Token[String] : %s(%lu, %lu)\n", string.data, strlen(string.data), string.size);

    SpjJSONValue jsonvalue;
    spj_jsonvalue_init(&jsonvalue, SpjJSONValueString);

    jsonvalue.type = SpjJSONValueString;
    jsonvalue.value.string = string;

    lexer->value = jsonvalue;

    assert(spj_lexer_peek(lexer) == '"');

    spj_lexer_increment(lexer);

    return SpjJSONTokenString;
}


SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer) {
    SpjJSONValue jsonvalue;

    spj_jsonvalue_init(&jsonvalue, SpjJSONValueNumber);

    size_t firstnumber_position = lexer->currentposition;

    char currentbyte = spj_lexer_peek(lexer);

    if ((isdigit(currentbyte) || currentbyte == '-') == 0) {
        return SpjJSONTokenError;
    }

    while ((currentbyte = spj_lexer_peek(lexer))) {
        if (isdigit(currentbyte) || currentbyte == '.') {
            spj_lexer_increment(lexer);
        } else {
            break;
        }
    }

    const char *firstchar = lexer->data + firstnumber_position;
    char *endpointer;

    jsonvalue.value.number = strtod(firstchar, &endpointer);

    // TODO

    int err = errno;
    if (jsonvalue.value.number == 0 && firstchar == endpointer) {
        assert(0);
    }

    if (err == ERANGE) {
        assert(0);
    }

    assert(endpointer == (lexer->data + lexer->currentposition));

    lexer->value = jsonvalue;

    printf("Token[Number] : %f\n", jsonvalue.value.number);

    assert(isdigit(spj_lexer_peek(lexer)) == 0);

    return SpjJSONTokenNumber;
}


SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer) {
    char currentbyte;

    while (isspace(currentbyte = spj_lexer_getc(lexer)));

    switch (currentbyte) {
        case '\0':
            return SpjJSONTokenEOS;

        case '[':
            return SpjJSONTokenArrayStart;

        case ']':
            return SpjJSONTokenArrayEnd;

        case '{': {
            return SpjJSONTokenObjectStart;
        }
        case '}':
            return SpjJSONTokenObjectEnd;

        case ':':
            return SpjJSONTokenColon;

        case ',':
            return SpjJSONTokenComma;

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

            printf("Token[Bool] : true\n");

            SpjJSONValue jsonvalue;
            spj_jsonvalue_init(&jsonvalue, SpjJSONValueBool);

            jsonvalue.value.number = 1;

            lexer->value = jsonvalue;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'f') {
        if (spj_lexer_getc(lexer) == 'a' &&
            spj_lexer_getc(lexer) == 'l' &&
            spj_lexer_getc(lexer) == 's' &&
            spj_lexer_getc(lexer) == 'e') {

            printf("Token[Bool] : false\n");

            SpjJSONValue jsonvalue;
            spj_jsonvalue_init(&jsonvalue, SpjJSONValueBool);

            jsonvalue.value.number = 0;

            lexer->value = jsonvalue;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'n') {
        if (spj_lexer_getc(lexer) == 'u' &&
            spj_lexer_getc(lexer) == 'l' &&
            spj_lexer_getc(lexer) == 'l') {
            
            printf("Token[Null]\n");

            SpjJSONValue jsonvalue;
            spj_jsonvalue_init(&jsonvalue, SpjJSONValueNull);

            lexer->value = jsonvalue;
            
            return SpjJSONTokenNull;
            
        } else {
            assert(0);
        }
    }
    
    
    // We should not reach here
    
    assert(0);
    
    return SpjJSONTokenError;
}

