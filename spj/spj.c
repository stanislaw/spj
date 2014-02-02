#include "spj.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;
} spj_iterator_t;


typedef struct {
    spj_iterator_t *iterator;
    union SpjJSONValue value;
} spj_lexer_t;


typedef enum SpjJSONTokenType {
    SpjJSONTokenError,
    SpjJSONTokenNumber,
    SpjJSONTokenString,
    SpjJSONTokenBool,     // true false
    SpjJSONTokenNull,     // null
    SpjJSONTokenObjectStart, // {
    SpjJSONTokenArrayStart, // [
    SpjJSONTokenObjectEnd,   // }
    SpjJSONTokenArrayEnd,   // ]
    SpjJSONTokenColon,    // : =
    SpjJSONTokenComma,    // ,
    SpjJSONTokenEOS //
} SpjJSONTokenType;


static char spj_iterator_peek(spj_iterator_t *iterator);

static spj_iterator_t spj_iterator_create(const char *jsonbytes, size_t datasize) {
    spj_iterator_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static char spj_iterator_getc(spj_iterator_t *iterator) {
    char c = iterator->data[iterator->currentposition];

    if (c) {
        iterator->currentposition++;
    }

    return c;
}


static void spj_iterator_consume_whitespace(spj_iterator_t *iterator) {
    while (isspace(spj_iterator_peek(iterator))) {
        spj_iterator_getc(iterator);
    }
}

static size_t __attribute__((unused)) spj_iterator_seek(spj_iterator_t *iterator, int offset) {
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


static char spj_iterator_peek(spj_iterator_t *iterator) {
   return iterator->data[iterator->currentposition];
}


static SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    char currentbyte = spj_iterator_getc(iterator);
    SpjString string;

    assert(currentbyte == '"');

    size_t firstchar_position = iterator->currentposition;

    while ((currentbyte = spj_iterator_getc(iterator)) != '"') {

    }

    char firstchar = iterator->data[firstchar_position];

    string.data = (char *)malloc(iterator->currentposition - firstchar_position);
    string.size = iterator->currentposition - firstchar_position;

    string.data = strncpy(string.data, iterator->data + firstchar_position, iterator->currentposition - firstchar_position - 1);

    printf("Token[String] : %s\n", string.data);

    lexer->value.string = string;
    
    assert(currentbyte == '"');

    return SpjJSONTokenString;
}


static SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    size_t firstnumber_position = iterator->currentposition;

    char currentbyte = spj_iterator_getc(iterator);

    if ((isdigit(currentbyte) || currentbyte == '-') == 0) {
        return SpjJSONTokenError;
    }

    while ((currentbyte = spj_iterator_getc(iterator))) {
        //printf("--- currentbyte %c\n", currentbyte);

        if ((isdigit(currentbyte) || currentbyte == '.') == 0) {
            break;
        }
    }

    spj_iterator_seek(iterator, -1);

    const char *pointer1 = iterator->data + firstnumber_position;
    const char *pointer2 = iterator->data + iterator->currentposition;

    double number = strtod(pointer1, &pointer2);

    lexer->value.number = number;

    printf("Token[Number] : %f\n", number);

    //printf("currentbyte %c\n", spj_iterator_peek(iterator));

    spj_iterator_seek(iterator, -1);

    assert(isdigit(spj_iterator_peek(iterator)));

    return SpjJSONTokenNumber;
}


// Для всех лексем, кроме Object и Array лексер возвращает (в аргументе lex) значение, которое парсер просто копирует в элемент контейнера.
static SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer) {
    SpjJSONTokenType jtokentype;

    spj_iterator_t *iterator = lexer->iterator;

    spj_iterator_consume_whitespace(iterator);

    char currentbyte = spj_iterator_peek(iterator);

    switch (currentbyte) {
        case '\0':
            return SpjJSONTokenEOS;

        case '[':
            return SpjJSONTokenArrayStart;

        case ']':
            return SpjJSONTokenArrayEnd;

        case '{':
            return SpjJSONTokenObjectStart;

        case '}':
            return SpjJSONTokenObjectEnd;

        case ',':
            return SpjJSONTokenComma;

        case '"':
            return spj_gettoken_string(lexer);
    }

    if (isdigit(currentbyte) || currentbyte == '-') {
        return spj_gettoken_number(lexer);
    }

    if (currentbyte == 't') {
        spj_iterator_seek(iterator, 1);
        if (spj_iterator_getc(iterator) == 'r' &&
            spj_iterator_getc(iterator) == 'u' &&
            spj_iterator_getc(iterator) == 'e') {

            printf("Token[Bool] : true\n");

            lexer->value.number = 1;
            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'f') {
        spj_iterator_seek(iterator, 1);
        if (spj_iterator_getc(iterator) == 'a' &&
            spj_iterator_getc(iterator) == 'l' &&
            spj_iterator_getc(iterator) == 's' &&
            spj_iterator_getc(iterator) == 'e') {

            printf("Token[Bool] : false\n");

            lexer->value.number = 0;
            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'n') {
        spj_iterator_seek(iterator, 1);
        if (spj_iterator_getc(iterator) == 'u' &&
            spj_iterator_getc(iterator) == 'l' &&
            spj_iterator_getc(iterator) == 'l') {

            printf("Token[Null]\n");

            return SpjJSONTokenNull;

        } else {
            assert(0);
        }
    }


    // We should not reach here
    assert(0);

    return jtokentype;
}


static SpjJSONParsingResult spj_parse_object(spj_lexer_t *lexer, SpjJSONData *jsondata) {
    spj_iterator_t *iterator = lexer->iterator;

    char currentbyte = spj_iterator_getc(iterator);

    assert(currentbyte == '{');

    return 0;
}


static SpjJSONParsingResult spj_parse_array(spj_lexer_t *lexer, SpjJSONData *jsondata) {
    spj_iterator_t *iterator = lexer->iterator;

    return 0;
}


static SpjJSONParsingResult spj_parse_normal(spj_lexer_t *lexer, SpjJSONData *jsondata) {
    SpjJSONParsingResult result = SpjJSONParsingResultSuccess;

    spj_iterator_t *iterator = lexer->iterator;

    spj_iterator_consume_whitespace(iterator);

    char currentbyte;
    while ((currentbyte = spj_iterator_getc(iterator))) {
        SpjJSONTokenType tokentype = spj_gettoken(lexer);

        switch (tokentype) {
            case SpjJSONTokenObjectStart: {
                result = spj_parse_object(lexer, jsondata);

                break;
            }

            case SpjJSONTokenArrayStart: {
                result = spj_parse_array(lexer, jsondata);

                break;
            }

            default:
                result = SpjJSONParsingResultError;

                break;
        }
    }

    return result;
}


SpjJSONParsingResult spj_parse(const char *jsonstring, SpjJSONData *jsondata) {
    SpjJSONParsingResult result = SpjJSONParsingResultSuccess;

    spj_lexer_t lexer;
    spj_iterator_t iterator;

    printf("Original JSON: %s\n", jsonstring);
    
    size_t datasize = strlen(jsonstring);

    iterator = spj_iterator_create(jsonstring, datasize);
    lexer.iterator = &iterator;

    spj_iterator_consume_whitespace(&iterator);

    char currentbyte;
    while ((currentbyte = spj_iterator_getc(&iterator))) {
        SpjJSONTokenType tokentype = spj_gettoken(&lexer);

        switch (tokentype) {
            case SpjJSONTokenObjectStart: {
                result = spj_parse_object(&lexer, jsondata);

                break;
            }

            case SpjJSONTokenArrayStart: {
                result = spj_parse_array(&lexer, jsondata);

                break;
            }

            default:
                assert(0);

                result = SpjJSONParsingResultError;
                
                break;
        }
    }

    return result;
}


int spj_delete (SpjJSONData *object) {
    return 0;
}

