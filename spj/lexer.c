

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


#pragma mark
#pragma mark Iterator


spj_iterator_t spj_iterator_create(const char *jsonbytes, size_t datasize) {
    spj_iterator_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static void spj_iterator_increment(spj_iterator_t *iterator) {
    iterator->currentposition++;

    // Check datasize = 0 case later
    if (iterator->currentposition == (iterator->datasize + 1) && iterator->datasize > 0) {
        print_callstack();
        assert(0);
    }
}


char spj_iterator_peek(spj_iterator_t *iterator) {
    return iterator->data[iterator->currentposition];
}


static char spj_iterator_getc(spj_iterator_t *iterator) {
    char c = iterator->data[iterator->currentposition];

    spj_iterator_increment(iterator);

    return c;
}


static size_t spj_iterator_seek(spj_iterator_t *iterator, int offset) {
    if (offset > 0) {
        iterator->currentposition += offset;

        if (iterator->currentposition > iterator->datasize - 1) {
            iterator->currentposition = iterator->datasize - 1;
        }
    } else if (offset < 0) {
        unsigned int uoffset = (unsigned int)(-offset);

        if (iterator->currentposition > uoffset) {
            iterator->currentposition -= uoffset;
        } else {
            iterator->currentposition = 0;
        }
    }

    return iterator->currentposition;
}


#pragma mark
#pragma mark Lexer functions


SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    SpjString string;

    assert(iterator->data[iterator->currentposition - 1] == '"');

    size_t firstchar_position = iterator->currentposition;

    while (spj_iterator_peek(iterator) != '"') {
        spj_iterator_increment(iterator);
    }

    string.size = iterator->currentposition - firstchar_position;
    string.data = (char *)malloc(string.size + 1);

    const char *firstchar_ptr = iterator->data + firstchar_position;

    memcpy(string.data, firstchar_ptr, string.size);
    string.data[string.size] = '\0';

    printf("Token[String] : %s(%lu, %lu)\n", string.data, strlen(string.data), string.size);

    lexer->value.string = string;
    lexer->value.type = SpjJSONValueString;

    assert(spj_iterator_peek(iterator) == '"');

    spj_iterator_increment(iterator);

    return SpjJSONTokenString;
}


SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    size_t firstnumber_position = iterator->currentposition;

    char currentbyte = spj_iterator_peek(iterator);

    if ((isdigit(currentbyte) || currentbyte == '-') == 0) {
        return SpjJSONTokenError;
    }

    while ((currentbyte = spj_iterator_peek(iterator))) {
        if (isdigit(currentbyte) || currentbyte == '.') {
            spj_iterator_increment(iterator);
        } else {
            break;
        }
    }

    const char *firstchar = iterator->data + firstnumber_position;
    char *endpointer;

    double number = strtod(firstchar, &endpointer);

    // TODO

    int err = errno;
    if (number == 0 && firstchar == endpointer) {
        assert(0);
    }

    if (err == ERANGE) {
        assert(0);
    }

    assert(endpointer == (iterator->data + iterator->currentposition));

    lexer->value.number = number;
    lexer->value.type = SpjJSONValueNumber;

    printf("Token[Number] : %f\n", number);

    assert(isdigit(spj_iterator_peek(iterator)) == 0);

    return SpjJSONTokenNumber;
}


SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer) {
    char currentbyte;

    spj_iterator_t *iterator = lexer->iterator;

    while (isspace(currentbyte = spj_iterator_getc(iterator)));

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
        spj_iterator_seek(iterator, -1);

        return spj_gettoken_number(lexer);
    }

    if (currentbyte == 't') {
        if (spj_iterator_getc(iterator) == 'r' &&
            spj_iterator_getc(iterator) == 'u' &&
            spj_iterator_getc(iterator) == 'e') {

            printf("Token[Bool] : true\n");

            lexer->value.number = 1;
            lexer->value.type = SpjJSONValueBool;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'f') {
        if (spj_iterator_getc(iterator) == 'a' &&
            spj_iterator_getc(iterator) == 'l' &&
            spj_iterator_getc(iterator) == 's' &&
            spj_iterator_getc(iterator) == 'e') {

            printf("Token[Bool] : false\n");

            lexer->value.number = 0;
            lexer->value.type = SpjJSONValueBool;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'n') {
        if (spj_iterator_getc(iterator) == 'u' &&
            spj_iterator_getc(iterator) == 'l' &&
            spj_iterator_getc(iterator) == 'l') {
            
            printf("Token[Null]\n");
            
            lexer->value.type = SpjJSONValueNull;
            
            return SpjJSONTokenNull;
            
        } else {
            assert(0);
        }
    }
    
    
    // We should not reach here
    
    assert(0);
    
    return SpjJSONTokenError;
}

