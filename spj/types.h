
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
    struct spj_jsonnamedvalue_t *data;
    size_t size;

#ifdef USE_OBJ_DICT
    struct htab *hash;
#endif
} SpjObject;


typedef struct SpjArray {
    struct spj_jsonvalue_t *data;
    size_t size;
} SpjArray;


typedef struct SpjString {
    char *data;
    size_t size;
} SpjString;


static const SpjObject SpjObjectZero = { NULL, 0 };
static const SpjArray  SpjArrayZero  = { NULL, 0 };
static const SpjString SpjStringZero = { NULL, 0 };


typedef struct spj_jsonvalue_t {
    spj_jsonvalue_type_t type;
    union {
        struct SpjObject object;
        struct SpjArray  array;
        struct SpjString string;
        double number;
    } value;
} spj_jsonvalue_t;


typedef struct spj_jsonnamedvalue_t {
    struct spj_jsonvalue_t value;
    struct SpjString name;
} spj_jsonnamedvalue_t;


typedef enum {
    /* TODO */
    SpjJSONNoError = 0,
    SpjJSONErrorWrongRootObject,
    SpjJSONErrorOutOfMemory,
    SpjJSONErrorUnexpectedEOS
} spj_errorcode_t;


typedef enum {
    SpjJSONParsingResultError = 0,
    SpjJSONParsingResultSuccess = 1
} spj_result_t;


typedef struct {
    spj_errorcode_t code;
    char *message;
} spj_error_t;


#endif

