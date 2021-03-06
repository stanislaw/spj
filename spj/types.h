
#ifndef spj_types_h
#define spj_types_h


#include <stdio.h>


typedef enum spj_jsonvalue_type_t {
    SpjValueObject = 1,
    SpjValueArray,
    SpjValueString,
    SpjValueNumber,
    SpjValueNull,
    SpjValueFalse,
    SpjValueTrue
} spj_jsonvalue_type_t;


typedef struct spj_string_t {
    char *data;
    size_t size;
} spj_string_t;


typedef struct spj_object_t {
    struct spj_string_t *keys;
    struct spj_jsonvalue_t *values;
    size_t size;
} spj_object_t;


typedef struct spj_array_t {
    struct spj_jsonvalue_t *values;
    size_t size;
} spj_array_t;


static const spj_object_t SpjObjectZero = { NULL, NULL, 0 };
static const spj_array_t  SpjArrayZero  = { NULL, 0 };
static const spj_string_t SpjStringZero = { NULL, 0 };


typedef struct spj_jsonvalue_t {
    spj_jsonvalue_type_t type;
    union {
        struct spj_object_t object;
        struct spj_array_t  array;
        struct spj_string_t string;
        double number;
    } value;
} spj_jsonvalue_t;


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

