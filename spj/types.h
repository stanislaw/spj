
#ifndef spj_types_h
#define spj_types_h


#include <stdio.h>


typedef enum spj_jsonvalue_type_t {
    SpjJSONValueObject = 1,
    SpjJSONValueArray,
    SpjJSONValueString,
    SpjJSONValueNumber,
    SpjJSONValueBool,
    SpjJSONValueNull
} spj_jsonvalue_type_t;


typedef struct SpjObject {
    struct SpjJSONNamedValue *data;
    size_t size;

#ifdef USE_OBJ_DICT
    struct htab *hash;
#endif
} SpjObject;


typedef struct SpjArray {
    struct SpjJSONValue *data;
    size_t size;
} SpjArray;


typedef struct SpjString {
    char *data;
    size_t size;
} SpjString;


static const SpjObject SpjObjectZero = { NULL, 0 };
static const SpjArray  SpjArrayZero  = { NULL, 0 };
static const SpjString SpjStringZero = { NULL, 0 };


typedef struct SpjJSONValue {
    spj_jsonvalue_type_t type;
    union {
        struct SpjObject object;
        struct SpjArray  array;
        struct SpjString string;
        double number;
    } value;
} SpjJSONValue;


typedef struct SpjJSONNamedValue {
    struct SpjJSONValue value;
    struct SpjString name;
} SpjJSONNamedValue;


typedef enum {
    /* TODO */
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


#endif

