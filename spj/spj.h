

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


void spj_parse(const char *jsonbytes, int datasize);

