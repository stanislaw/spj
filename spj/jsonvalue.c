

#import "jsonvalue.h"

#include <assert.h>
#include <stdlib.h>


void spj_jsonvalue_init(SpjJSONValue *jsonvalue, SpjJSONValueType type) {
    jsonvalue->type = type;

    switch (type) {
        case SpjJSONValueObject:
            jsonvalue->object = SpjObjectZero;

            break;

        case SpjJSONValueArray:
            jsonvalue->array = SpjArrayZero;

            break;

        case SpjJSONValueString:
            jsonvalue->string = SpjStringZero;

            break;

        case SpjJSONValueNumber:
            jsonvalue->number = 0;

            break;

        case SpjJSONValueBool:
            jsonvalue->number = 0;

            break;

        default:
            break;
    }
}


void spj_jsonvalue_object_add(SpjJSONValue *jsonvalue, SpjJSONNamedValue *child_jsonvalue, size_t *capacity) {
    const size_t slice = 10;

    assert(jsonvalue->type == SpjJSONValueObject); // remove later

    if (jsonvalue->object.size == *capacity) {
        SpjJSONNamedValue *larger_objects_data = realloc(jsonvalue->object.data, (*capacity += slice) * sizeof(SpjJSONNamedValue));

        if (larger_objects_data != NULL) {
            jsonvalue->object.data = larger_objects_data;
        } else {
            assert(0); // TODO nomem
        }
    }

    jsonvalue->object.data[jsonvalue->object.size++] = *child_jsonvalue;
}


void spj_jsonvalue_array_add(SpjJSONValue *jsonvalue, SpjJSONValue *child_jsonvalue, size_t *capacity) {
    const size_t slice = 10;

    assert(jsonvalue->type == SpjJSONValueArray); // remove later

    if (jsonvalue->array.size == *capacity) {
        SpjJSONValue *larger_array_data = realloc(jsonvalue->array.data, (*capacity += slice) * sizeof(SpjJSONValue));

        if (larger_array_data != NULL) {
            jsonvalue->array.data = larger_array_data;
        } else {
            assert(0); // TODO
        }
    }

    jsonvalue->array.data[jsonvalue->array.size++] = *child_jsonvalue;
}


void spj_jsonvalue_object_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->object.size < *capacity) {
        SpjJSONNamedValue *data = realloc(jsonvalue->object.data, jsonvalue->object.size * sizeof(SpjJSONNamedValue));
        jsonvalue->object.data = data;
    }
}


void spj_jsonvalue_array_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->array.size < *capacity) {
        SpjJSONValue *data = realloc(jsonvalue->array.data, jsonvalue->array.size * sizeof(SpjJSONValue));
        jsonvalue->array.data = data;
    }
}


void spj_jsonvalue_enumerate(SpjJSONValue *jsonvalue, int (*function)(SpjJSONValue *jsonvalue)) {
    size_t i;

    switch (jsonvalue->type) {
        case SpjJSONValueArray:
            for (i = 0; i < jsonvalue->array.size; i++) {
                SpjJSONValue data = jsonvalue->array.data[i];

                function(&data);
            }

            break;

        case SpjJSONValueObject:
            for (i = 0; i < jsonvalue->object.size; i++) {
                //SpjJSONNamedValue data = jsonvalue->object.data[i];

                //function(&data);
            }

            break;

        case SpjJSONValueString:
            //function(data);

            break;

        case SpjJSONValueNumber:

            break;

        case SpjJSONValueNull:

            break;
            
        case SpjJSONValueBool:

            break;
            
        default:
            
            assert(0);
            
            break;
    }
}

int spj_jsonvalue_free(SpjJSONValue *jsonvalue) {
    if (jsonvalue == NULL) {
        return 0;
    }

    switch (jsonvalue->type) {
        case SpjJSONValueObject:
            // здесь нужно вызывать spj_jsonvalue_delete рекурсивно? (для краткости лучше тут нужна функция траверса)

            break;

        case SpjJSONValueArray:
            // здесь нужно вызывать spj_jsonvalue_delete рекурсивно? (для краткости лучше тут нужна функция траверса)
            
            break;

        case SpjJSONValueString:
            jsonvalue->string = SpjStringZero;

            break;

        case SpjJSONValueNumber:
            jsonvalue->number = 0;

            break;

        case SpjJSONValueBool:
            jsonvalue->number = 0;

            break;

        default:
            break;
    }

    return 0;
}

