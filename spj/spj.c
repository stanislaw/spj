#include "spj.h"

#include <stdio.h>
#include <string.h>


static spj_iter_t spj_iter_create(const char *jsonbytes, size_t datasize) {
    spj_iter_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static int spj_iter_getc(spj_iter_t *iterator) {
    int c = (iterator->currentposition > iterator->datasize - 1) ? 0 
            : iterator->data[iterator->currentposition];

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


int spj_parse (const char *json_str, jVal *root) {
    size_t datasize = strlen(json_str);

    spj_iter_t iterator = spj_iter_create(json_str, datasize);

    while (spj_iter_peek(&iterator)) {
        printf("%c", spj_iter_getc(&iterator));
    }

    return 0;
}


int spj_parse2 (const char *json_str, size_t jsize, jVal *root) {
    return 0;
}

int spj_delete (jVal *obj) {
    return 0;
}


