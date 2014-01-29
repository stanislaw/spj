
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
  int size;
#ifdef USE_OBJ_DICT
  struct htab *hash;
#endif
} jObject;


typedef struct jArray {
  struct jval *data; // array
  int size;
} jArray;


typedef struct jString {
  char *data; // nil terminating
  int size;
} jString;


union value {
  struct jObject obj;
  struct jArray arr;
  struct jString str;
  double number;  // Bool, Null, integer and real (all in one)
};


typedef struct jVal {
  jValue_type type;    // try to do it without flags
  struct jval *parent; // calculated AFTER the filling of the PARENT's container
  char *name;          // name pairs (valid only for members of the object)
  union value value;
} jVal;


typedef struct spj_iter {
   const char *data;
   int currentposition,
       datasize;
} spj_iter_t;


void spj_parse(const char *jsonbytes, int datasize);

