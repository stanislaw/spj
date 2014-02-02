#include "spj.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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


SpjJSONTokenType spj_gettoken_string(spj_iterator_t *iterator, spj_lexer_t *lexer) {
    char currentbyte = spj_iterator_getc(iterator);
    SpjString string;

    assert(currentbyte == '"');

    size_t firstchar_position = iterator->currentposition;

    while ((currentbyte = spj_iterator_getc(iterator)) != '"') {

    }

    char firstchar = iterator->data[firstchar_position];

//    printf("--- %c %d\n", firstchar, firstchar_position);
//    printf("--- %c %d\n", currentbyte, iterator->currentposition);

    string.data = (char *)malloc(iterator->currentposition - firstchar_position);
    string.size = iterator->currentposition - firstchar_position;

    string.data = strncpy(string.data, iterator->data + firstchar_position, iterator->currentposition - firstchar_position - 1);

    printf("Token[String] : %s\n", string.data);

    lexer->value.string = string;
    
    assert(currentbyte == '"');

    return SpjJSONTokenString;
}


SpjJSONTokenType spj_gettoken_number(spj_iterator_t *iterator, spj_lexer_t *lexer) {
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

SpjJSONTokenType spj_gettoken(spj_iterator_t *iterator, spj_lexer_t *lexer) {
    SpjJSONTokenType jtokentype;

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

        case '"':
            return spj_gettoken_string(iterator, lexer);
    }

    if (isdigit(currentbyte)) {
        return spj_gettoken_number(iterator, lexer);
    }

    return jtokentype;
}


SpjJSONParsingResult spj_parse_object(spj_iterator_t *iterator, SpjJSONData *jsondata) {

    return 0;
}


SpjJSONParsingResult spj_parse_array(spj_iterator_t *iterator, SpjJSONData *jsondata) {

    return 0;
}


SpjJSONParsingResult spj_parse(const char *json_str, SpjJSONData *jsondata) {
    SpjJSONParsingResult result = SpjJSONParsingResultSuccess;

    printf("Original JSON: %s\n", json_str);
    
    size_t datasize = strlen(json_str);

    spj_iterator_t iterator = spj_iterator_create(json_str, datasize);
    spj_lexer_t lexer;

    spj_iterator_consume_whitespace(&iterator);

    char currentbyte;
    while ((currentbyte = spj_iterator_getc(&iterator))) {
        //printf("%c", currentbyte);

        switch (spj_gettoken(&iterator, &lexer)) {
            case SpjJSONTokenObjectStart: {
                result = spj_parse_object(&iterator, jsondata);

                break;
            }

            case SpjJSONTokenObjectEnd: {
                result = spj_parse_array(&iterator, jsondata);

                break;
            }

            default:
                result = SpjJSONParsingResultError;

                break;
            // error если на верхнем уровне разрешены только Object и Array
        }
    }

    return 0;
}


int spj_delete (SpjJSONData *object) {
    return 0;
}

