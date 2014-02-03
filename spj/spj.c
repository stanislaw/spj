#include "spj.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;
} spj_iterator_t;


typedef struct {
    spj_iterator_t *iterator;
    union SpjJSONValue value;
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



static SpjJSONTokenType spj_parse_default(spj_lexer_t *lexer, SpjJSONData *jsondata, int inarray);



static spj_iterator_t spj_iterator_create(const char *jsonbytes, size_t datasize) {
    spj_iterator_t iterator;

    iterator.data = jsonbytes;
    iterator.currentposition = 0;
    iterator.datasize = datasize;

    return iterator;
}


static void spj_iterator_increment(spj_iterator_t *iterator) {
    iterator->currentposition++;
}


static char spj_iterator_peek(spj_iterator_t *iterator) {
    return iterator->data[iterator->currentposition];
}


static char spj_iterator_getc(spj_iterator_t *iterator) {
    char c = iterator->data[iterator->currentposition];

    spj_iterator_increment(iterator);

    return c;
}


static size_t __attribute__((unused)) spj_iterator_seek(spj_iterator_t *iterator, int offset) {
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


static SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer) {
    //printf("spj_gettoken_string begins\n");

    spj_iterator_t *iterator = lexer->iterator;

    SpjString string;

    assert(iterator->data[iterator->currentposition - 1] == '"');

    size_t firstchar_position = iterator->currentposition;

    while (spj_iterator_peek(iterator) != '"') {
        spj_iterator_increment(iterator);
    }

    char firstchar = iterator->data[firstchar_position];


    //printf("%d %d, %c %c\n", firstchar_position, iterator->currentposition, firstchar, spj_iterator_peek(iterator) );

    string.data = (char *)malloc(iterator->currentposition - firstchar_position + 1);
    string.size = iterator->currentposition - firstchar_position + 1;

    string.data = strncpy(string.data, iterator->data + firstchar_position, iterator->currentposition - firstchar_position);

    printf("Token[String] : %s\n", string.data);

    lexer->value.string = string;
    
    assert(spj_iterator_peek(iterator) == '"');

    spj_iterator_increment(iterator);

    return SpjJSONTokenString;
}


static SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer) {
    spj_iterator_t *iterator = lexer->iterator;

    //printf("spj_gettoken_number begins %c: \n", spj_iterator_peek(iterator));

    size_t firstnumber_position = iterator->currentposition;

    char currentbyte = spj_iterator_peek(iterator);

    if ((isdigit(currentbyte) || currentbyte == '-') == 0) {
        return SpjJSONTokenError;
    }

    while ((currentbyte = spj_iterator_peek(iterator))) {
        //printf("--- currentbyte %c\n", currentbyte);

        if (isdigit(currentbyte) || currentbyte == '.') {
            spj_iterator_increment(iterator);
        } else {
            break;
        }
    }

    const char *firstchar = iterator->data + firstnumber_position;
    const char *endpointer;

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

    printf("Token[Number] : %f\n", number);

    assert(isdigit(spj_iterator_peek(iterator)) == 0);

    return SpjJSONTokenNumber;
}


static SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer) {
    //printf("spj_gettoken begins\n");

    SpjJSONTokenType jtokentype;
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

            return SpjJSONTokenNull;

        } else {
            assert(0);
        }
    }


    // We should not reach here

    assert(0);

    return SpjJSONTokenError;
}


static spj_result_t spj_parse_object(spj_lexer_t *lexer, SpjJSONData *jsondata) {
    //printf("spj_parse_object begins\n");

    size_t n;


    SpjJSONData * objects_data = malloc(20 * sizeof(SpjJSONData)); // learn how to reallocate... VladD: "arena..."
    spj_iterator_t *iterator = lexer->iterator;

    SpjJSONTokenType token;

    //assert(spj_iterator_peek(iterator) == '{');


    for (n = 0;;) {
        token = spj_gettoken(lexer);

        if (n == 0 && token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenString) {
            printf("expected strig %s\n", iterator->data + iterator->currentposition);

            assert(0);
        }

        SpjJSONData object_data;
        object_data.name = lexer->value.string.data;


        if ((token = spj_gettoken(lexer)) != SpjJSONTokenColon) {
            assert(0);
        }


        spj_parse_default(lexer, &object_data, 0);

        n++;

        objects_data[n] = object_data;

        token = spj_gettoken(lexer);

        if (token == SpjJSONTokenObjectEnd) {
            break;
        }

        if (token != SpjJSONTokenComma) {
            assert(0);
        }
    }

    assert(iterator->data[iterator->currentposition - 1] == '}');

    SpjObject object;

    object.data = objects_data;
    object.size = n;

    jsondata->value.object = object;

    return 0;
}


static spj_result_t spj_parse_array(spj_lexer_t *lexer, SpjJSONData *jsondata) {
    //printf("spj_parse_array begins\n");

    spj_iterator_t *iterator = lexer->iterator;

    SpjJSONData * array_data = malloc(20 * sizeof(SpjJSONData)); // learn how to reallocate... VladD... arena....

    SpjJSONTokenType token;

    assert(iterator->data[iterator->currentposition - 1] == '[');

    size_t n;

    for (n = 0;;) {
        SpjJSONData object_data;

        token = spj_parse_default(lexer, &object_data, 1);

        if (token == SpjJSONTokenArrayEnd) {
            break;
        }

        n++;

        array_data[n] = object_data;

        token = spj_gettoken(lexer);

        if (token == SpjJSONTokenArrayEnd) {
            break;
        }

        if (token != SpjJSONTokenComma) {
            assert(0);
        }
    }

    SpjArray array;

    array.data = array_data;
    array.size = n;

    jsondata->value.array = array;

    assert(iterator->data[iterator->currentposition - 1] == ']');

    return 0;
}


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

// Parses end data, but also skips to ], } and ,
static SpjJSONTokenType spj_parse_default(spj_lexer_t *lexer, SpjJSONData *jsondata, int inarray) {
    //printf("spj_parse_normal begins\n");

    spj_iterator_t *iterator = lexer->iterator;

    SpjJSONTokenType token = spj_gettoken(lexer);

    switch (token) {
        case SpjJSONTokenObjectStart: {
            spj_parse_object(lexer, jsondata);
            jsondata->type = SpjJSONValueObject;

            break;
        }

        case SpjJSONTokenArrayStart: {
            spj_parse_array(lexer, jsondata);
            jsondata->type = SpjJSONValueArray;

            break;
        }

        case SpjJSONTokenNumber: case SpjJSONTokenString: case SpjJSONTokenBool: case SpjJSONTokenNull: {
            jsondata->type = spj_value_for_token(token);
            jsondata->value = lexer->value;

            break;
        }

        // Legitimate exception for Empty Array not to perform lookups twice
        case SpjJSONTokenArrayEnd: {
            if (inarray) break;
        }

        default:
            printf("current byte is %c, %d of total %d", spj_iterator_peek(iterator), iterator->currentposition, iterator->datasize);
            printf("what is the token %d\n", token);

            assert(0);
    }

    return token;
}


spj_result_t spj_parse(const char *jsonstring, SpjJSONData *jsondata, spj_error_t *error) {
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
            jsondata->type = SpjJSONValueObject;

            result = spj_parse_object(&lexer, jsondata);

            break;
        }

        case SpjJSONTokenArrayStart: {
            jsondata->type = SpjJSONValueArray;

            result = spj_parse_array(&lexer, jsondata);

            break;
        }

        default:
            //*error.code = 0;
            error->message = "JSON contains no valid root element (Object, Array)";

            return SpjJSONParsingResultError;
    }

    return result;
}


int spj_delete (SpjJSONData *object) {
    return 0;
}


void spj_jsondata_debug(SpjJSONData *jsondata) {
    //printf("jsondata->type: %d\n", jsondata->type);

    /*
     traverse here...
    while (jsondata->)
     */

}
