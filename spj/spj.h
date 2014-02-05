#include <stdio.h>


#ifdef USE_OBJ_DICT
struct htab; // will be implemented sometime
#endif


typedef enum SpjJSONValueType {
  SpjJSONValueObject = 1,
  SpjJSONValueArray,
  SpjJSONValueString,
  SpjJSONValueNumber,
  SpjJSONValueBool,
  SpjJSONValueNull
} SpjJSONValueType;


typedef struct SpjObject {
    struct SpjJSONNamedValue *data; // array
    size_t size;

#ifdef USE_OBJ_DICT
      struct htab *hash;
#endif
} SpjObject;


typedef struct SpjArray {
    struct SpjJSONValue *data; // array
    size_t size;
} SpjArray;


typedef struct SpjString {
    char *data; // nil terminating
    size_t size;
} SpjString;


typedef struct SpjJSONValue {
    SpjJSONValueType type;
    union
    {
        struct SpjObject object;
        struct SpjArray  array;
        struct SpjString string;
        double number;   // Bool, Null, integer and real (all in one)
    };
} SpjJSONValue;


typedef struct SpjJSONNamedValue { // was: SpjJSONValue
    struct SpjJSONValue value;
    struct SpjString name;
} SpjJSONNamedValue;

typedef enum {
    SpjJSONNoError = 0,
    SpjJSONErrorWrongRootObject,
    SpjJSONErrorOutOfMemory,
    SpjJSONErrorUnexpectedEOS
} SpjJSONError;


typedef enum {
    SpjJSONParsingResultError = 0,
    SpjJSONParsingResultSuccess = 1
} spj_result_t;


typedef struct {
    SpjJSONError code;
    char *message;
} spj_error_t;


spj_result_t spj_parse(const char *jsonstring, SpjJSONValue *jsonvalue, spj_error_t *error);
int spj_delete (SpjJSONValue *jsonvalue);


void spj_jsonvalue_debug(SpjJSONValue *jsonvalue);






