#include "spj.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct {
    const char *data;
    size_t currentposition;
    size_t datasize;
} spj_iterator_t;


typedef struct {
    spj_iterator_t *iterator;
    union SpjJSONValue value;
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



static SpjJSONParsingResult spj_parse_normal(spj_lexer_t *lexer, SpjJSONData *jsondata, spj_error_t *error);



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


static char spj_iterator_getc(spj_iterator_t *iterator) {
    char c = iterator->data[iterator->currentposition];

    return c;
}


static char spj_iterator_getc_and_increment(spj_iterator_t *iterator) {
    char c = iterator->data[iterator->currentposition];

    spj_iterator_increment(iterator);

    return c;
}


static void spj_iterator_consume_whitespace(spj_iterator_t *iterator) {
    while (isspace(spj_iterator_peek(iterator))) {
        spj_iterator_increment(iterator);
    }
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


static char spj_iterator_peek(spj_iterator_t *iterator) {
   return iterator->data[iterator->currentposition];
}


static SpjJSONTokenType spj_gettoken_string(spj_lexer_t *lexer, spj_error_t *error) {
    printf("spj_gettoken_string begins\n");

    spj_iterator_t *iterator = lexer->iterator;

    SpjString string;

    assert(spj_iterator_getc(iterator) == '"');

    spj_iterator_increment(iterator);
    size_t firstchar_position = iterator->currentposition;

    while (spj_iterator_getc(iterator) != '"') {
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

    return SpjJSONTokenString;
}


static SpjJSONTokenType spj_gettoken_number(spj_lexer_t *lexer, spj_error_t *error) {
    spj_iterator_t *iterator = lexer->iterator;

    printf("spj_gettoken_number begins %c: \n", spj_iterator_getc(iterator));

    size_t firstnumber_position = iterator->currentposition;

    char currentbyte = spj_iterator_getc(iterator);

    if ((isdigit(currentbyte) || currentbyte == '-') == 0) {
        return SpjJSONTokenError;
    }

    while ((currentbyte = spj_iterator_getc(iterator))) {
        //printf("--- currentbyte %c\n", currentbyte);

        if (isdigit(currentbyte) || currentbyte == '.') {
            spj_iterator_increment(iterator);
        } else {
            break;
        }
    }

    const char *pointer1 = iterator->data + firstnumber_position;
    const char *pointer2 = iterator->data + iterator->currentposition;

    double number = strtod(pointer1, &pointer2);

    lexer->value.number = number;

    printf("Token[Number] : %f\n", number);

    //printf("currentbyte %c\n", spj_iterator_peek(iterator));

    spj_iterator_seek(iterator, -1);

    assert(isdigit(spj_iterator_peek(iterator)));

    return SpjJSONTokenNumber;
}


// Для всех лексем, кроме Object и Array лексер возвращает (в аргументе lex) значение, которое парсер просто копирует в элемент контейнера.
static SpjJSONTokenType spj_gettoken(spj_lexer_t *lexer, spj_error_t *error) {
    printf("spj_gettoken begins\n");

    SpjJSONTokenType jtokentype;


    spj_iterator_t *iterator = lexer->iterator;

    spj_iterator_consume_whitespace(iterator);

    char currentbyte = spj_iterator_peek(iterator);

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
            assert(0);

        case ',':
            return SpjJSONTokenComma;

        case '"':
            return spj_gettoken_string(lexer, error);
    }

    if (isdigit(currentbyte) || currentbyte == '-') {
        return spj_gettoken_number(lexer, error);
    }

    if (currentbyte == 't') {
        spj_iterator_seek(iterator, 1);
        if (spj_iterator_getc_and_increment(iterator) == 'r' &&
            spj_iterator_getc_and_increment(iterator) == 'u' &&
            spj_iterator_getc_and_increment(iterator) == 'e') {

            printf("Token[Bool] : true\n");

            spj_iterator_seek(iterator, -1);

            lexer->value.number = 1;

            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'f') {
        spj_iterator_seek(iterator, 1);
        if (spj_iterator_getc_and_increment(iterator) == 'a' &&
            spj_iterator_getc_and_increment(iterator) == 'l' &&
            spj_iterator_getc_and_increment(iterator) == 's' &&
            spj_iterator_getc_and_increment(iterator) == 'e') {

            printf("Token[Bool] : false\n");

            spj_iterator_seek(iterator, -1);

            lexer->value.number = 0;
            return SpjJSONTokenBool;

        } else {
            assert(0);
        }
    }

    if (currentbyte == 'n') {
        spj_iterator_seek(iterator, 1);
        if (spj_iterator_getc_and_increment(iterator) == 'u' &&
            spj_iterator_getc_and_increment(iterator) == 'l' &&
            spj_iterator_getc_and_increment(iterator) == 'l') {
            printf("Token[Null]\n");


            spj_iterator_seek(iterator, -1);

            return SpjJSONTokenNull;

        } else {
            assert(0);
        }
    }


    // We should not reach here
    assert(0);

    return jtokentype;
}


static SpjJSONParsingResult spj_parse_object(spj_lexer_t *lexer, SpjJSONData *jsondata, spj_error_t *error) {
    printf("spj_parse_object begins\n");

    SpjJSONData * objects_data = malloc(20 * sizeof(SpjJSONData)); // learn how to reallocate... VladD: "arena..."

    spj_iterator_t *iterator = lexer->iterator;

    assert(spj_iterator_getc(iterator) == '{');
    spj_iterator_increment(iterator);

    size_t i = 0;

    spj_iterator_consume_whitespace(iterator); // eat whitespace before } or name

    while (spj_iterator_getc(iterator) != '}') {
        spj_iterator_consume_whitespace(iterator);

        assert(spj_iterator_getc(iterator) == '"');

        SpjJSONTokenType tokentype = spj_gettoken_string(lexer, error);

        assert(tokentype == SpjJSONTokenString);

        SpjJSONData object_data;
        object_data.name = lexer->value.string.data;

        assert(spj_iterator_getc(iterator) == '"');

        spj_iterator_increment(iterator);
        spj_iterator_consume_whitespace(iterator);

        assert(spj_iterator_getc(iterator) == ':');


        spj_iterator_increment(iterator);
        SpjJSONParsingResult value_result = spj_parse_normal(lexer, &object_data, error);

        objects_data[i] = object_data;

        spj_iterator_increment(iterator);
        spj_iterator_consume_whitespace(iterator);

        assert(spj_iterator_getc(iterator) == '}' || spj_iterator_getc(iterator) == ',');

        if (spj_iterator_getc(iterator) == ',') {
            spj_iterator_increment(iterator);
        }

        i++;
    }

    assert(spj_iterator_getc(iterator) == '}');

    SpjObject object;

    object.data = objects_data;
    object.size = i;

    jsondata->value.object = object;

    return 0;
}


static SpjJSONParsingResult spj_parse_array(spj_lexer_t *lexer, SpjJSONData *jsondata, spj_error_t *error) {
    printf("spj_parse_array begins\n");
    spj_iterator_t *iterator = lexer->iterator;

    SpjJSONData * array_data = malloc(20 * sizeof(SpjJSONData)); // learn how to reallocate... VladD... arena....

    assert(spj_iterator_getc(iterator) == '[');

    spj_iterator_increment(iterator);

    size_t i = 0;

    while (spj_iterator_getc(iterator) != ']') {
        SpjJSONData object_data;

        SpjJSONParsingResult value_result = spj_parse_normal(lexer, &object_data, error);

        array_data[i] = object_data;

        spj_iterator_increment(iterator);
        spj_iterator_consume_whitespace(iterator);

        assert(spj_iterator_getc(iterator) == ']' || spj_iterator_getc(iterator) == ',');

        if (spj_iterator_getc(iterator) == ',') {
            spj_iterator_increment(iterator);
        }

        i++;
    }

    
    assert(spj_iterator_getc(iterator) == ']');


    SpjArray array;

    array.data = array_data;
    array.size = i;


    jsondata->value.array = array;


    return 0;
}


static SpjJSONParsingResult spj_parse_normal(spj_lexer_t *lexer, SpjJSONData *jsondata, spj_error_t *error) {
    printf("spj_parse_normal begins\n");

    SpjJSONParsingResult result = SpjJSONParsingResultSuccess;

    spj_iterator_t *iterator = lexer->iterator;

    spj_iterator_consume_whitespace(iterator);

    while (spj_iterator_getc(iterator)) {
        SpjJSONTokenType tokentype = spj_gettoken(lexer, error);

        switch (tokentype) {
            case SpjJSONTokenObjectStart: {
                spj_parse_object(lexer, jsondata, error);
                jsondata->type = SpjJSONValueObject;

                return result;
            }

            case SpjJSONTokenArrayStart: {
                spj_parse_array(lexer, jsondata, error);
                jsondata->type = SpjJSONValueArray;

                return result;
            }

            case SpjJSONTokenNumber: {
                // fill number into jsondata
                jsondata->type = SpjJSONValueArray;
                jsondata->value.number = lexer->value.number;

                return result;
            }

            case SpjJSONTokenString: {
                jsondata->type = SpjJSONValueString;
                jsondata->value.string = lexer->value.string;

                return result;
            }


            case SpjJSONTokenBool: {
                jsondata->type = SpjJSONValueBool;
                jsondata->value.number = lexer->value.number;

                return result;
            }


            case SpjJSONTokenNull: {
                jsondata->type = SpjJSONValueNull;

                return result;
            }


            default:
                printf("current byte is %c, %d of total %d", spj_iterator_getc(iterator), iterator->currentposition, iterator->datasize);
                printf("what is the token %d\n", tokentype);
                assert(0);
        }
    }

    return result;
}


SpjJSONParsingResult spj_parse(const char *jsonstring, SpjJSONData *jsondata, spj_error_t *error) {
    SpjJSONParsingResult result;

    spj_lexer_t lexer;
    spj_iterator_t iterator;

    printf("Original JSON: %s\n", jsonstring);
    
    size_t datasize = strlen(jsonstring);

    iterator = spj_iterator_create(jsonstring, datasize);
    lexer.iterator = &iterator;

    SpjJSONTokenType tokentype = spj_gettoken(&lexer, error);

    switch (tokentype) {
        case SpjJSONTokenObjectStart: {
            jsondata->type = SpjJSONValueObject;

            result = spj_parse_object(&lexer, jsondata, error);

            break;
        }

        case SpjJSONTokenArrayStart: {
            jsondata->type = SpjJSONValueArray;

            result = spj_parse_array(&lexer, jsondata, error);

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
    printf("jsondata->type: %d\n", jsondata->type);

    /*
     traverse here...
    while (jsondata->)
     */

}
