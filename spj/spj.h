#include <stdio.h>


struct SpjJSONData;


#ifdef USE_OBJ_DICT
struct htab; // will be implemented sometime
#endif


typedef enum SpjJSONValueType {
  SpjJSONValueObject,
  SpjJSONValueArray,
  SpjJSONValueString,
  SpjJSONValueNumber,
  SpjJSONValueBool,
  SpjJSONValueNull
} SpjJSONValueType;


typedef struct SpjObject {
  struct SpjJSONData *data; // array
  size_t size;
#ifdef USE_OBJ_DICT
  struct htab *hash;
#endif
} SpjObject;


typedef struct SpjArray {
    struct SpjJSONData *data; // array
    size_t size;
} SpjArray;


typedef struct SpjString {
    char *data; // nil terminating
    size_t size;
} SpjString;


union SpjJSONValue {
    struct SpjObject object;
    struct SpjArray  array;
    struct SpjString string;
    
    double number;   // Bool, Null, integer and real (all in one)
};


typedef struct SpjJSONData {
    SpjJSONValueType type;
    // struct jval *parent;   // calculated AFTER the filling of the PARENT's container
    char *name;               // name pairs (valid only for members of the object)
    union SpjJSONValue value;
} SpjJSONData;


typedef enum {
    SpjJSONParsingResultError = 0,
    SpjJSONParsingResultSuccess = 0
} SpjJSONParsingResult;


SpjJSONParsingResult spj_parse (const char *jsonstring, SpjJSONData *jsondata);
int spj_delete (SpjJSONData *jsondata);

