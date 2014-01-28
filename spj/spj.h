

struct jval {
  long type_and_flags;
  struct jval *parent, *next, *prev;
  char *name;
  union value;
};


typedef struct spj_iter {
   const char *data;
   int currentposition,
       datasize;
} spj_iter_t;


static inline spj_iter_t spj_iter_create(const char *jsonbytes, int datasize) {
    spj_iter_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}

// возвращает символ, увеличивает итератор
static inline int spj_getc(spj_iter_t *iterator) {
    char c = *iterator->data;

    // (iterator->data)++; // ???
    (iterator->currentposition)++;

    return c;
}

// изменяет итератор на offset позиций, возвращает currentPosition
static inline int spj_seek(spj_iter_t *iterator, int offset) {
    int _offset = 0;

    while (_offset < offset) {
        // (iterator->data)++; //  ???
        (iterator->currentposition)++;

        _offset++;
    }

    return iterator->currentposition;
}


static inline int spj_peek (spj_iter_t *iterator) {
   return iterator->data[iterator->currentposition];
}


static inline void spj_parse(const char *jsonbytes, int datasize) {
    printf("spj_parse todo: %s...\n", jsonbytes);
}


