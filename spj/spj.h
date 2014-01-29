#include <stdio.h>

struct jval;
#ifdef USE_OBJ_DICT
struct htab; // will be implemented sometime
#endif


typedef enum jValue_type {
  Object,
  Array,
  String,
  Number,
  Bool,
  Null
} jValue_type;


typedef struct jObject {
  struct jval *data; // array
  size_t size;
#ifdef USE_OBJ_DICT
  struct htab *hash;
#endif
} jObject;


typedef struct jArray {
  struct jval *data; // array
  size_t size;
} jArray;


typedef struct jString {
  char *data; // nil terminating
  size_t size;
} jString;


union value {
  struct jObject obj;
  struct jArray arr;
  struct jString str;
  double number;  // Bool, Null, integer and real (all in one)
};


typedef struct jval {
  jValue_type type;    // try to do it without flags
  struct jval *parent; // calculated AFTER the filling of the PARENT's container
  char *name;          // name pairs (valid only for members of the object)
  union value value;
} jval;


typedef struct spj_iter {
   const char *data;
   size_t currentposition;
   size_t datasize;
} spj_iter_t;


int spj_parse (const char *json_str, jval *root);
int spj_parse2 (const char *json_str, size_t jsize, jval *root);
int spj_delete (jval *obj);




