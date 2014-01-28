#include "spj.h"

#include <stdio.h>

spj_iter_t spj_iter_create(const char *jsonbytes, int datasize) {
    spj_iter_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}

// возвращает символ, увеличивает итератор
int spj_iter_getc(spj_iter_t *iterator) {
    int c = iterator->data[iterator->currentposition];

    if (c) {
        iterator->currentposition++;
    } else {
        c = EOF; // конец строки json. curpos оставим на месте 
    }

    return c;
}

// изменяет итератор на offset позиций, возвращает currentPosition
int spj_iter_seek(spj_iter_t *iterator, int offset) {
    iterator->currentposition += offset; // проверим допустимость новой позиции. Она д.б. внутри строки json.

    if (iterator->currentposition > iterator->datasize - 1) {
        iterator->currentposition = iterator->datasize - 1; // это 0 в конце строки
    } else if (iterator->currentposition < 0) {
        iterator->currentposition = 0;
    }

    return iterator->currentposition;
}


int spj_iter_peek (spj_iter_t *iterator) {
   return iterator->data[iterator->currentposition];
}


void spj_parse(const char *jsonbytes, int datasize) {
    spj_iter_t iterator = spj_iter_create(jsonbytes, datasize);

    while (spj_iter_peek(&iterator)) {
        printf("%c", spj_iter_getc(&iterator));
    }
}



