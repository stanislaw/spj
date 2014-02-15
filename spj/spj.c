#include "spj.h"

#include "lexer.h"
#include "jsonvalue.h"

#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static spj_jsontoken_type_t spj_parse_default(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue, int inarray);


static spj_result_t spj_parse_object(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue) {
    spj_jsontoken_type_t token;
    spj_jsonvalue_t child_jsonvalue_value;

    assert(lexer->data[lexer->currentposition - 1] == '{'); /* remove this later */

    for (;;) {
        token = spj_gettoken(lexer);

        if (jsonvalue->value.object.size == 0 && token == SpjTokenObjectEnd) {
            break;
        }

        if (token != SpjTokenString) {
            printf("expected string %s\n", lexer->data + lexer->currentposition);

            assert(0);
        }

        jsonvalue->value.object.size++;

        size_t idx = lexer->buf_bytes_used + jsonvalue->value.object.size - 1;

        lexer->keys_buf[idx] = lexer->value.value.string;
        
        if ((token = spj_gettoken(lexer)) != SpjTokenColon) {
            assert(0);
        }

        lexer->buf_bytes_used += jsonvalue->value.object.size;
        spj_parse_default(lexer, &child_jsonvalue_value, 0);
        lexer->buf_bytes_used -= jsonvalue->value.object.size;

        lexer->values_buf[idx] = child_jsonvalue_value;


        token = spj_gettoken(lexer);

        if (token == SpjTokenObjectEnd) {
            break;
        }


        if (token != SpjTokenComma) {
            assert(0);
        }
    }

    assert(lexer->data[lexer->currentposition - 1] == '}');


    if (jsonvalue->value.object.size > 0) {
        jsonvalue->value.object.values = malloc(jsonvalue->value.object.size * sizeof(spj_jsonvalue_t));
        memcpy(jsonvalue->value.object.values, lexer->values_buf + lexer->buf_bytes_used, jsonvalue->value.object.size * sizeof(spj_jsonvalue_t));

        jsonvalue->value.object.keys = malloc(jsonvalue->value.object.size * sizeof(spj_string_t));
        memcpy(jsonvalue->value.object.keys, lexer->keys_buf + lexer->buf_bytes_used, jsonvalue->value.object.size * sizeof(spj_string_t));
    }

    /* spj_jsonvalue_object_finalize(jsonvalue, &capacity); */

    return 0;
}


static spj_result_t spj_parse_array(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue) {
    spj_jsontoken_type_t token;
    spj_jsonvalue_t child_jsonvalue;

    assert(lexer->data[lexer->currentposition - 1] == '[');

    for (;;) {
        lexer->buf_bytes_used += jsonvalue->value.array.size;
        token = spj_parse_default(lexer, &child_jsonvalue, jsonvalue->value.array.size == 0);
        lexer->buf_bytes_used -= jsonvalue->value.array.size;

        if (jsonvalue->value.array.size == 0 && token == SpjTokenArrayEnd) {
            break;
        }

        jsonvalue->value.array.size++;

        size_t idx = lexer->buf_bytes_used + jsonvalue->value.array.size - 1;

        lexer->values_buf[idx] = child_jsonvalue;


        token = spj_gettoken(lexer);

        if (token == SpjTokenArrayEnd) {
            break;
        }

        if (token != SpjTokenComma) {
            printf("fail on symbol %d %c\n", token, spj_lexer_peek(lexer));
            
            assert(0);
        }
    }

    assert(lexer->data[lexer->currentposition - 1] == ']');

    if (jsonvalue->value.array.size > 0) {
        jsonvalue->value.array.values = malloc(jsonvalue->value.array.size * sizeof(spj_jsonvalue_t));

        memcpy(jsonvalue->value.array.values, lexer->values_buf + lexer->buf_bytes_used, jsonvalue->value.array.size * sizeof(spj_jsonvalue_t));
    }

    return 0;
}


static spj_jsontoken_type_t spj_parse_default(spj_lexer_t *lexer, spj_jsonvalue_t *jsonvalue, int inarray) {
    spj_jsontoken_type_t token = spj_gettoken(lexer);

    switch (token) {
        case SpjTokenObjectStart: {
            spj_jsonvalue_init(jsonvalue, SpjValueObject);

            spj_parse_object(lexer, jsonvalue);

            break;
        }

        case SpjTokenArrayStart: {
            spj_jsonvalue_init(jsonvalue, SpjValueArray);

            spj_parse_array(lexer, jsonvalue);

            break;
        }

        case SpjTokenNumber: case SpjTokenString: case SpjTokenTrue: case SpjTokenFalse: case SpjTokenNull: {
            *jsonvalue = lexer->value;

            break;
        }

        /* Legitimate exception for Empty Array not to perform lookups twice */
        case SpjTokenArrayEnd: {
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

    /*printf("Original JSON: %s\n", jsonstring);*/

    lexer = spj_lexer_create(jsonstring, datasize);
    
    lexer.error = error;

    while (isspace(spj_lexer_peek(&lexer))) {
        spj_lexer_increment(&lexer);
    }

    switch (spj_lexer_getc(&lexer)) {
        case '{': {
            spj_jsonvalue_init(jsonvalue, SpjValueObject);

            result = spj_parse_object(&lexer, jsonvalue);

            break;
        }

        case '[': {
            spj_jsonvalue_init(jsonvalue, SpjValueArray);

            result = spj_parse_array(&lexer, jsonvalue);

            break;
        }

        default:
            error->message = "JSON contains no valid root element (Object, Array)";

            return SpjJSONParsingResultError;
    }

    spj_lexer_free(&lexer);
    
    return result;
}

