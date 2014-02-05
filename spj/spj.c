#include "spj.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <execinfo.h>

#ifdef __APPLE__
// https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/backtrace.3.html
#define print_callstack() { \
    void *callstack[128]; \
    int i, frames = backtrace(callstack, 128); \
    char **strs = backtrace_symbols(callstack, frames); \
    printf("Callstack:\n"); \
    for (i = 0; i < frames; ++i) { \
        printf("%s\n", strs[i]); \
    } \
    free(strs); \
}
#else
#define print_callstack() ((void)0)
#endif


typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;
} spj_iterator_t;


typedef struct {
    spj_iterator_t *iterator;
    SpjJSONValue value;
    spj_error_t *error;
} spj_lexer_t;


typedef enum SpjJSONTokenType {
    SpjJSONTokenError =       0,
    SpjJSONTokenNumber =      1,
    SpjJSONTokenString =      2,
    SpjJSONTokenBool =        3, // true false
    SpjJSONTokenNull =        4, // null
    SpjJSONTokenObjectStart = 5, // {
    SpjJSONTokenArrayStart =  6, // [
    SpjJSONTokenObjectEnd =   7, // }
    SpjJSONTokenArrayEnd =    8, // ]
    SpjJSONTokenColon =       9, // : =
    SpjJSONTokenComma =      10, // ,
    SpjJSONTokenEOS =        11 //
} SpjJSONTokenType;


static char spj_iterator_peek(spj_iterator_t *iterator);


static SpjJSONTokenType spj_parse_default(spj_lexer_t *lexer, SpjJSONValue *jsonvalue, int inarray);


SpjJSONValueType spj_value_for_token(SpjJSONTokenType token) {
    switch (token) {
        case SpjJSONTokenObjectStart: {
            return SpjJSONValueObject;
        }

        case SpjJSONTokenArrayStart: {
            return SpjJSONValueArray;
        }

        case SpjJSONTokenNumber: {
            return SpjJSONValueNumber;
        }

        case SpjJSONTokenString: {
            return SpjJSONValueString;
        }

        case SpjJSONTokenBool: {
            return SpjJSONValueBool;
        }

        case SpjJSONTokenNull: {
            return SpjJSONValueNull;
        }

        default:
            assert(0);
    }
    
    return 0;
}


#pragma mark
#pragma mark Iterator


static spj_iterator_t spj_iterator_create(const char *jsonbytes, size_t datasize) {
    spj_iterator_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static void spj_iterator_increment(spj_iterator_t *iterator) {
    iterator->currentposition++;

    // Check datasize = 0 case later
    if (iterator->currentposition == (iterator->datasize + 1) && iterator->datasize > 0) {
        print_callstack();
        assert(0);
    }
}


static char spj_iterator_peek(spj_iterator_t *iterator) {
    return iterator->data[iterator->currentposition];
}


static char spj_iterator_getc(spj_iterator_t *iterator) {
    char c = iterator->data[iterator->currentposition];

    spj_iterator_increment(iterator);

    return c;
}


static size_t spj_iterator_seek(spj_iterator_t *iterator, int offset) {
    if (offset > 0) {
        iterator->currentposition += offset;

        if (iterator->currentposition > iterator->datasize - 1) {
            iterator->currentposition = iterator->datasize - 1;
        }
    } else if (offset < 0) {
        unsigned int uoffset = (unsigned int)(-offset);

        if (iterator->currentposition > uoffset) {
            iterator->currentposition -= uoffset;
        } else {
            iterator->currentposition = 0;
        }
    }

    return iterator->currentposition;
}


#pragma mark
#pragma mark Lexer functions


static SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    SpjString string;

    assert(iterator->data[iterator->currentposition - 1] == '"');

    size_t firstchar_position = iterator->currentposition;

    while (spj_iterator_peek(iterator) != '"') {
        spj_iterator_increment(iterator);
    }

    string.size = iterator->currentposition - firstchar_position;
    string.data = (char *)malloc(string.size + 1);

    const char *firstchar_ptr = iterator->data + firstchar_position;

    memcpy(string.data, firstchar_ptr, string.size);
    string.data[string.size] = '\0';

    printf("Token[String] : %s(%lu, %lu)\n", string.data, strlen(string.data), string.size);

    lexer->value.string = string;
    lexer->value.type = SpjJSONValueString;

    assert(spj_iterator_peek(iterator) == '"');

    spj_iterator_increment(iterator);

    return SpjJSONTokenString;
}


static SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    size_t firstnumber_position = iterator->currentposition;

    char currentbyte = spj_iterator_peek(iterator);

    if ((isdigit(currentbyte) || currentbyte == '-') == 0) {
        return SpjJSONTokenError;
    }

    while ((currentbyte = spj_iterator_peek(iterator))) {
        if (isdigit(currentbyte) || currentbyte == '.') {
            spj_iterator_increment(iterator);
        } else {
            break;
        }
    }

    const char *firstchar = iterator->data + firstnumber_position;
    char *endpointer;

    double number = strtod(firstchar, &endpointer);

    // TODO

    int err = errno;
    if (number == 0 && firstchar == endpointer) {
        assert(0);
    }

    if (err == ERANGE) {
        assert(0);
    }

    assert(endpointer == (iterator->data + iterator->currentposition));

    lexer->value.number = number;
    lexer->value.type = SpjJSONValueNumber;

    printf("Token[Number] : %f\n", number);

    assert(isdigit(spj_iterator_peek(iterator)) == 0);

    return SpjJSONTokenNumber;
}


static SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer) {
    char currentbyte;

    spj_iterator_t *iterator = lexer->iterator;

    while (isspace(currentbyte = spj_iterator_getc(iterator)));

    switch (currentbyte) {
        case '\0':
            return SpjJSONTokenEOS;

        case '[':
            return SpjJSONTokenArrayStart;

        case ']':
            return SpjJSONTokenArrayEnd;

        case '{': {
            return SpjJSONTokenObjectStart;
        }
        case '}':
            return SpjJSONTokenObjectEnd;

        case ':':
            return SpjJSONTokenColon;

        case ',':
            return SpjJSONTokenComma;

        case '"':
            return spj_gettoken_string(lexer);
    }

    if (isdigit(currentbyte) || currentbyte == '-') {
        spj_iterator_seek(iterator, -1);

        return spj_gettoken_number(lexer);
    }

    if (currentbyte == 't') {
        if (spj_iterator_getc(iterator) == 'r' &&
            spj_iterator_getc(iterator) == 'u' &&
            spj_iterator_getc(iterator) == 'e') {

            printf("Token[Bool] : true\n");

            lexer->value.number = 1;
            lexer->value.type = SpjJSONValueBool;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'f') {
        if (spj_iterator_getc(iterator) == 'a' &&
            spj_iterator_getc(iterator) == 'l' &&
            spj_iterator_getc(iterator) == 's' &&
            spj_iterator_getc(iterator) == 'e') {

            printf("Token[Bool] : false\n");

            lexer->value.number = 0;
            lexer->value.type = SpjJSONValueBool;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'n') {
        if (spj_iterator_getc(iterator) == 'u' &&
            spj_iterator_getc(iterator) == 'l' &&
            spj_iterator_getc(iterator) == 'l') {

            printf("Token[Null]\n");

            lexer->value.type = SpjJSONValueNull;

            return SpjJSONTokenNull;

        } else {
            assert(0);
        }
    }


    // We should not reach here

    assert(0);

    return SpjJSONTokenError;
}


#pragma mark
#pragma mark SpjJSONValue manipulation


static void spj_jsonvalue_init(SpjJSONValue *jsonvalue, SpjJSONValueType type) {
    jsonvalue->type = type;

    jsonvalue->object.data = NULL;
    jsonvalue->object.size = 0;

    jsonvalue->array.data = NULL;
    jsonvalue->array.size = 0;

    jsonvalue->number = 0;
}


static void spj_jsonvalue_object_add(SpjJSONValue *jsonvalue, SpjJSONNamedValue *child_jsonvalue, size_t *capacity) {
    const size_t slice = 10;

    assert(jsonvalue->type == SpjJSONValueObject); // remove later

    if (jsonvalue->object.size == *capacity) {
        SpjJSONNamedValue *larger_objects_data = realloc(jsonvalue->object.data, *capacity += slice);

        if (larger_objects_data != NULL) {
            jsonvalue->object.data = larger_objects_data;
        } else {
            assert(0); // TODO nomem
        }
    }

    jsonvalue->object.data[jsonvalue->object.size++] = *child_jsonvalue;

    printf("size is %lu %s\n", jsonvalue->object.size, child_jsonvalue->value.string.data);

    for (size_t i = 0; i < jsonvalue->object.size; i++) {
        /*
         Original JSON: {"hello":"world", "key":"value", "number": "one"}
         Token[String] : hello(5, 5)
         Token[String] : world(5, 5)
         size is 1 world
         world
         Token[String] : key(3, 3)
         Token[String] : value(5, 5)
         size is 2
         make: *** [test] Segmentation fault: 11
         */


        printf("%s\n", jsonvalue->object.data[i].value.string.data); // <- Segmentation fault: 11
    }
}


static void spj_jsonvalue_array_add(SpjJSONValue *jsonvalue, SpjJSONValue *child_jsonvalue, size_t *capacity) {
    const size_t slice = 10;

    assert(jsonvalue->type == SpjJSONValueArray); // remove later

    if (jsonvalue->array.size == *capacity) {
        SpjJSONValue *larger_array_data = realloc(jsonvalue->array.data, *capacity += slice);

        if (larger_array_data != NULL) {
            jsonvalue->array.data = larger_array_data;
        } else {
            assert(0); // TODO
        }
    }

    jsonvalue->array.data[jsonvalue->array.size++] = *child_jsonvalue;
}


static void spj_jsonvalue_object_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->object.size < *capacity) {
        SpjJSONNamedValue *data = realloc(jsonvalue->object.data, jsonvalue->object.size);
        jsonvalue->object.data = data;
    }
}


static void spj_jsonvalue_array_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->array.size < *capacity) {
        SpjJSONValue *data = realloc(jsonvalue->array.data, jsonvalue->array.size);
        jsonvalue->array.data = data;
    }
}


#pragma mark
#pragma mark Parsing functions


static spj_result_t spj_parse_object(spj_lexer_t *lexer, SpjJSONValue *jsonvalue) {
    //printf("spj_parse_object begins\n");

    size_t n, capacity;
    SpjJSONTokenType token;
    spj_iterator_t *iterator;
    SpjJSONNamedValue child_jsonvalue;
    SpjJSONValue child_jsonvalue_value;


    capacity = 0; // initial capacity, to be modified by spj_jsonvalue_object_add


    iterator = lexer->iterator;

    // remove this later
    assert(iterator->data[iterator->currentposition - 1] == '{');

    assert(jsonvalue->type == SpjJSONValueObject);
    assert(jsonvalue->object.size == 0);

    for (n = 0;; n++) {
        token = spj_gettoken(lexer);

        if (n == 0 && token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenString) {
            printf("expected string %s\n", iterator->data + iterator->currentposition);

            assert(0);
        }

        child_jsonvalue.name = lexer->value.string;

        if ((token = spj_gettoken(lexer)) != SpjJSONTokenColon) {
            assert(0);
        }

        spj_parse_default(lexer, &child_jsonvalue_value, 0);

        child_jsonvalue.value = child_jsonvalue_value;

        spj_jsonvalue_object_add(jsonvalue, &child_jsonvalue, &capacity);

        token = spj_gettoken(lexer);

        if (token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenComma) {
            assert(0);
        }
    }

    assert(iterator->data[iterator->currentposition - 1] == '}');

    spj_jsonvalue_object_finalize(jsonvalue, &capacity);


    return 0;
}


static spj_result_t spj_parse_array(spj_lexer_t *lexer, SpjJSONValue *jsonvalue) {
    size_t i, capacity;
    spj_iterator_t *iterator;
    SpjJSONTokenType token;
    SpjJSONValue child_jsonvalue;

    iterator = lexer->iterator;

    capacity = 0; // initial capacity for array, to be modified by spj_jsonvalue_array_add

    assert(jsonvalue->type == SpjJSONValueArray);
    assert(jsonvalue->array.size == 0);

    // remove this later
    assert(iterator->data[iterator->currentposition - 1] == '[');

    for (i = 0;; i++) {
        SpjJSONValue object_data;

        spj_jsonvalue_init(&object_data, SpjJSONValueArray);

        token = spj_parse_default(lexer, &child_jsonvalue, 1);

        if (i == 0 && token == SpjJSONTokenArrayEnd) {
            break;
        }

        spj_jsonvalue_array_add(jsonvalue, &child_jsonvalue, &capacity);

        token = spj_gettoken(lexer);

        if (token == SpjJSONTokenArrayEnd) {
            break;
        }

        if (token != SpjJSONTokenComma) {
            assert(0);
        }
    }

    // remove this later
    assert(iterator->data[iterator->currentposition - 1] == ']');

    spj_jsonvalue_array_finalize(jsonvalue, &capacity);

    return 0;
}


// Parses end data, but also skips to ], } and ,
static SpjJSONTokenType spj_parse_default(spj_lexer_t *lexer, SpjJSONValue *jsonvalue, int inarray) {
    //printf("spj_parse_normal begins\n");

    spj_iterator_t *iterator = lexer->iterator;

    SpjJSONTokenType token = spj_gettoken(lexer);

    switch (token) {
        case SpjJSONTokenObjectStart: {
            spj_jsonvalue_init(jsonvalue, SpjJSONValueObject);

            spj_parse_object(lexer, jsonvalue);

            break;
        }

        case SpjJSONTokenArrayStart: {
            spj_jsonvalue_init(jsonvalue, SpjJSONValueArray);

            spj_parse_array(lexer, jsonvalue);

            break;
        }

        case SpjJSONTokenNumber: case SpjJSONTokenString: case SpjJSONTokenBool: case SpjJSONTokenNull: {
            *jsonvalue = lexer->value;

            break;
        }

        // Legitimate exception for Empty Array not to perform lookups twice
        case SpjJSONTokenArrayEnd: {
            if (inarray) break;
        }

        default:
            printf("current byte is %c, %lu of total %lu", spj_iterator_peek(iterator), iterator->currentposition, iterator->datasize);
            printf("what is the token %d\n", token);

            assert(0);
    }

    return token;
}


#pragma mark
#pragma mark Public API


spj_result_t spj_parse(const char *jsonstring, SpjJSONValue *jsonvalue, spj_error_t *error) {
    spj_result_t result;
    spj_lexer_t lexer;
    spj_iterator_t iterator;
    size_t datasize;

    printf("Original JSON: %s\n", jsonstring);
    
    datasize = strlen(jsonstring);

    iterator = spj_iterator_create(jsonstring, datasize);
    lexer.iterator = &iterator;

    lexer.error = error;

    switch (spj_gettoken(&lexer)) {
        case SpjJSONTokenObjectStart: {
            spj_jsonvalue_init(jsonvalue, SpjJSONValueObject);

            result = spj_parse_object(&lexer, jsonvalue);

            break;
        }

        case SpjJSONTokenArrayStart: {
            spj_jsonvalue_init(jsonvalue, SpjJSONValueArray);

            result = spj_parse_array(&lexer, jsonvalue);

            break;
        }

        default:
            //*error.code = 0;
            error->message = "JSON contains no valid root element (Object, Array)";

            return SpjJSONParsingResultError;
    }

    return result;
}


int spj_delete (SpjJSONValue *object) {
    return 0;
}


void spj_jsonvalue_debug(SpjJSONValue *jsonvalue) {
    size_t i;

    printf("[Debug] ");

    switch (jsonvalue->type) {
        case SpjJSONValueArray:
            printf("[");

            for (i = 0; i < jsonvalue->array.size; i++) {
                SpjJSONValue data = jsonvalue->array.data[i];

                spj_jsonvalue_debug(&data);
            }

            printf("]");

            break;

        case SpjJSONValueObject:
            printf("{");

            for (i = 0; i < jsonvalue->object.size; i++) {
                SpjJSONNamedValue data = jsonvalue->object.data[i];

                printf("%s:-----", data.name.data);

                // TODO TODO TODO
                assert(0);
                //spj_jsonvalue_debug(&data);
            }

            printf("}");

            break;

        case SpjJSONValueString:
            printf("\"");

            printf("%s", jsonvalue->string.data);

            printf("\"");

            break;

        case SpjJSONValueNumber:
            //printf("\"");

            printf("%f", jsonvalue->number);

            break;

        case SpjJSONValueNull:
            printf("null");

            break;

        case SpjJSONValueBool:
            if (jsonvalue->number == 1) {
                printf("false");
            } else {
                printf("true");
            }

            break;

        default:

            assert(0);

            break;
    }

    printf("\n");
}
