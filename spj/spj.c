#include "spj.h"

#include "lexer.h"
#include "jsonvalue.h"

#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static SpjJSONTokenType spj_parse_default(spj_lexer_t *lexer, SpjJSONValue *jsonvalue, int inarray);

static spj_result_t spj_parse_object(spj_lexer_t *lexer, SpjJSONValue *jsonvalue) {
    size_t n, capacity;
    SpjJSONTokenType token;
    SpjJSONNamedValue child_jsonvalue;
    SpjJSONValue child_jsonvalue_value;


    capacity = 0;

    assert(lexer->data[lexer->currentposition - 1] == '{'); /* remove this later */

    assert(jsonvalue->type == SpjJSONValueObject);
    assert(jsonvalue->value.object.size == 0);

    for (n = 0;; n++) {
        token = spj_gettoken(lexer);

        if (n == 0 && token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenString) {
            printf("expected string %s\n", lexer->data + lexer->currentposition);

            assert(0);
        }

        child_jsonvalue.name = SpjStringZero;

        child_jsonvalue.name = lexer->value.value.string;

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

    assert(lexer->data[lexer->currentposition - 1] == '}');

    spj_jsonvalue_object_finalize(jsonvalue, &capacity);

    return 0;
}


static spj_result_t spj_parse_array(spj_lexer_t *lexer, SpjJSONValue *jsonvalue) {
    size_t i, capacity;
    SpjJSONTokenType token;
    SpjJSONValue child_jsonvalue;

    capacity = 0;

    assert(jsonvalue->type == SpjJSONValueArray);
    assert(jsonvalue->value.array.size == 0);

    assert(lexer->data[lexer->currentposition - 1] == '[');

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
            printf("fail on symbol %d %c\n", token, spj_lexer_peek(lexer));
            
            assert(0);
        }
    }

    assert(lexer->data[lexer->currentposition - 1] == ']');

    spj_jsonvalue_array_finalize(jsonvalue, &capacity);

    return 0;
}


static SpjJSONTokenType spj_parse_default(spj_lexer_t *lexer, SpjJSONValue *jsonvalue, int inarray) {
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


spj_result_t spj_parse(const char *jsonstring, size_t datasize, SpjJSONValue *jsonvalue, spj_error_t *error) {
    spj_result_t result;
    spj_lexer_t lexer;

    /* printf("Original JSON: %s\n", jsonstring); */
    

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

