#include "spj.h"

#include <stdio.h>
#include <string.h>


static spj_iterator_t spj_iterator_create(const char *jsonbytes, size_t datasize) {
    spj_iterator_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static int spj_iterator_getc(spj_iterator_t *iterator) {
    int c = (iterator->currentposition > iterator->datasize - 1) ? 0 
            : iterator->data[iterator->currentposition];

    if (c) {
        iterator->currentposition++;
    } else {
        c = EOF;
    }

    return c;
}


static void spj_iterator_consumewhitespace(spj_iterator_t *iterator) {
    while (isspace(spj_iterator_getc(iterator))) {}
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


static int spj_iterator_peek (spj_iterator_t *iterator) {
   return iterator->data[iterator->currentposition];
}



SpjJSONTokenType spj_getoken (spj_iterator_t *iterator, spj_lexer_t *lexer) {
    SpjJSONTokenType jtokentype;

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

    size_t datasize = strlen(json_str);

    spj_iterator_t iterator = spj_iterator_create(json_str, datasize);
    spj_lexer_t lexer;
    SpjJSONTokenType tokenType;

    while (spj_iterator_peek(&iterator)) {
        printf("%c", spj_iterator_getc(&iterator));

        switch (tokenType = spj_getoken(&iterator, &lexer)) {
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

