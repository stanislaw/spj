#include "spj.h"

#include <stdio.h>

static spj_iter_t spj_iter_create(const char *jsonbytes, int datasize) {
    spj_iter_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static int spj_iter_getc(spj_iter_t *iterator) {
    int c = iterator->data[iterator->currentposition];

    if (c) {
        iterator->currentposition++;
    } else {
        c = EOF;
    }

    return c;
}


static int __attribute__((unused)) spj_iter_seek(spj_iter_t *iterator, int offset) {
    iterator->currentposition += offset;


    if (iterator->currentposition > iterator->datasize - 1) {
        iterator->currentposition = iterator->datasize - 1;
    } else if (iterator->currentposition < 0) {
        iterator->currentposition = 0;
    }

    return iterator->currentposition;
}


static int spj_iter_peek (spj_iter_t *iterator) {
   return iterator->data[iterator->currentposition];
}


void spj_parse(const char *jsonbytes, int datasize) {
    spj_iter_t iterator = spj_iter_create(jsonbytes, datasize);

    while (spj_iter_peek(&iterator)) {
        printf("%c", spj_iter_getc(&iterator));
    }
}


