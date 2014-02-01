#include "spj.h"

#include <stdio.h>
#include <string.h>


static spj_iterator_t spj_iter_create(const char *jsonbytes, size_t datasize) {
    spj_iterator_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static int spj_iter_getc(spj_iterator_t *iterator) {
    int c = (iterator->currentposition > iterator->datasize - 1) ? 0 
            : iterator->data[iterator->currentposition];

    if (c) {
        iterator->currentposition++;
    } else {
        c = EOF;
    }

    return c;
}


static size_t __attribute__((unused)) spj_iter_seek(spj_iterator_t *iterator, int offset) {
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


static int spj_iter_peek (spj_iterator_t *iterator) {
   return iterator->data[iterator->currentposition];
}



SpjJSONTokenType spj_getoken (spj_iterator_t *iterator, spj_lexer_t *lexer) {
    SpjJSONTokenType jtokentype;

    return jtokentype;
}


int spj_parse (const char *json_str, SpjJSONData *root) {
    size_t datasize = strlen(json_str);

    spj_iterator_t iterator = spj_iter_create(json_str, datasize);
    spj_lexer_t lexer;
    SpjJSONTokenType tokenType;

    while (spj_iter_peek(&iterator)) {
        printf("%c", spj_iter_getc(&iterator));

        switch (tokenType = spj_getoken(&iterator, &lexer)) {
            case SpjJSONTokenObjectStart: {
                //rc = parse_obj(&iter, ptr_jval);
                break;
            }

            case SpjJSONTokenObjectEnd: {
                //rc = parse_arr(&iter, ptr_jval);
                break;
            }

            default:
                break;
            // error если на верхнем уровне разрешены только Object и Array
        }
        //return rc;
    }

    return 0;
}


int spj_delete (SpjJSONData *object) {
    return 0;
}

