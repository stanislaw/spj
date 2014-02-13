#include "spj.h"

#include "lexer.h"
#include "jsonvalue.h"

#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static spj_jsontoken_type_t spj_parse_default(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue, int inarray);


static spj_result_t spj_parse_object(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue) {
    size_t n, capacity;
    spj_jsontoken_type_t token;
    spj_jsonvalue_t child_jsonvalue_value;

    spj_jsonvalue_t *values = alloca(sizeof(spj_jsonvalue_t) * 20);
    spj_string_t *keys = alloca(sizeof(spj_string_t) * 20);

    capacity = 0;

    assert(lexer->data[lexer->currentposition - 1] == '{'); /* remove this later */

    assert(jsonvalue->type == SpjJSONValueObject);
    assert(jsonvalue->value.object.size == 0);
    assert(jsonvalue->value.object.values == NULL);

    for (n = 0;; n++) {
        token = spj_gettoken(lexer);

        if (n == 0 && token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenString) {
            printf("expected string %s\n", lexer->data + lexer->currentposition);

            assert(0);
        }

        keys[n] = lexer->value.value.string;
        
        if ((token = spj_gettoken(lexer)) != SpjJSONTokenColon) {
            assert(0);
        }

        spj_parse_default(lexer, &child_jsonvalue_value, 0);

        values[jsonvalue->value.object.size++] = child_jsonvalue_value;

        token = spj_gettoken(lexer);

        if (token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenComma) {
            assert(0);
        }
    }

    assert(lexer->data[lexer->currentposition - 1] == '}');


    if (jsonvalue->value.object.size > 0) {
        jsonvalue->value.object.values = malloc(jsonvalue->value.object.size * sizeof(spj_jsonvalue_t));
        memcpy(jsonvalue->value.object.values, values, jsonvalue->value.object.size * sizeof(spj_jsonvalue_t));

        jsonvalue->value.object.keys = malloc(jsonvalue->value.object.size * sizeof(spj_string_t));
        memcpy(jsonvalue->value.object.keys, keys, jsonvalue->value.object.size * sizeof(spj_string_t));
    }

    /* spj_jsonvalue_object_finalize(jsonvalue, &capacity); */

    return 0;
}


static spj_result_t spj_parse_array(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue) {
    size_t i, capacity;
    spj_jsontoken_type_t token;
    spj_jsonvalue_t child_jsonvalue;

    capacity = 0;

    assert(lexer->data[lexer->currentposition - 1] == '[');

    assert(jsonvalue->value.array.size == 0);
    assert(jsonvalue->value.array.values == NULL);
    assert(jsonvalue->type == SpjJSONValueArray);

    spj_jsonvalue_t *elements = malloc(sizeof(spj_jsonvalue_t) * 20);

    for (i = 0;; i++) {
        token = spj_parse_default(lexer, &child_jsonvalue, (i == 0));

        if (i == 0 && token == SpjJSONTokenArrayEnd) {
            break;
        }

        elements[jsonvalue->value.array.size++] = child_jsonvalue;

        token = spj_gettoken(lexer);

        if (token == SpjJSONTokenArrayEnd) {
            break;
        }

        if (token != SpjJSONTokenComma) {
            printf("fail on symbol %d %c\n", token, spj_lexer_peek(lexer));
            
            assert(0);
        }
    }

    assert(lexer->data[lexer->currentposition - 1] == ']');

    if (jsonvalue->value.array.size > 0) {
        jsonvalue->value.array.values = malloc(jsonvalue->value.array.size * sizeof(spj_jsonvalue_t));

        memcpy(jsonvalue->value.array.values, elements, jsonvalue->value.array.size * sizeof(spj_jsonvalue_t));
    }

    return 0;
}


static spj_jsontoken_type_t spj_parse_default(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue, int inarray) {
    spj_jsontoken_type_t token = spj_gettoken(lexer);

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

        /* Legitimate exception for Empty Array not to perform lookups twice */
        case SpjJSONTokenArrayEnd: {
            if (inarray) break;
        }
            
        default:
            /* TODO */
            assert(0);
    }
    
    return token;
}


#pragma mark
#pragma mark Public API


spj_result_t spj_parse(const char *jsonstring, size_t datasize, spj_jsonvalue_t *jsonvalue, spj_error_t *error) {
    spj_result_t result;
    spj_lexer_t lexer;

    //printf("Original JSON: %s\n", jsonstring);

    lexer = spj_lexer_create(jsonstring, datasize);
    
    lexer.error = error;

    while (isspace(spj_lexer_peek(&lexer))) {
        spj_lexer_increment(&lexer);
    }

    switch (spj_lexer_getc(&lexer)) {
        case '{': {
            spj_jsonvalue_init(jsonvalue, SpjJSONValueObject);

            result = spj_parse_object(&lexer, jsonvalue);

            break;
        }

        case '[': {
            spj_jsonvalue_init(jsonvalue, SpjJSONValueArray);

            result = spj_parse_array(&lexer, jsonvalue);

            break;
        }

        default:
            error->message = "JSON contains no valid root element (Object, Array)";

            return SpjJSONParsingResultError;
    }

    
    return result;
}

