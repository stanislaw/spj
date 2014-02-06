

#include "jsonvalue.h"

#include <assert.h>
#include <stdlib.h>


void spj_jsonvalue_init(SpjJSONValue *jsonvalue, SpjJSONValueType type) {
    jsonvalue->type = type;

    switch (type) {
        case SpjJSONValueObject:
            jsonvalue->value.object = SpjObjectZero;

            break;

        case SpjJSONValueArray:
            jsonvalue->value.array = SpjArrayZero;

            break;

        case SpjJSONValueString:
            jsonvalue->value.string = SpjStringZero;

            break;

        case SpjJSONValueNumber:
            jsonvalue->value.number = 0;

            break;

        case SpjJSONValueBool:
            jsonvalue->value.number = 0;

            break;

        default:
            break;
    }
}


void spj_jsonvalue_object_add(SpjJSONValue *jsonvalue, SpjJSONNamedValue *child_jsonvalue, size_t *capacity) {
    const size_t slice = 10;

    assert(jsonvalue->type == SpjJSONValueObject); /* remove later */

    if (jsonvalue->value.object.size == *capacity) {
        SpjJSONNamedValue *larger_objects_data = realloc(jsonvalue->value.object.data, (*capacity += slice) * sizeof(SpjJSONNamedValue));

        if (larger_objects_data != NULL) {
            jsonvalue->value.object.data = larger_objects_data;
        } else {
            assert(0); /* TODO nomem */
        }
    }

    jsonvalue->value.object.data[jsonvalue->value.object.size++] = *child_jsonvalue;
}


void spj_jsonvalue_array_add(SpjJSONValue *jsonvalue, SpjJSONValue *child_jsonvalue, size_t *capacity) {
    const size_t slice = 10;

    assert(jsonvalue->type == SpjJSONValueArray); /* remove later */

    if (jsonvalue->value.array.size == *capacity) {
        SpjJSONValue *larger_array_data = realloc(jsonvalue->value.array.data, (*capacity += slice) * sizeof(SpjJSONValue));

        if (larger_array_data != NULL) {
            jsonvalue->value.array.data = larger_array_data;
        } else {
            assert(0); /* TODO */
        }
    }

    jsonvalue->value.array.data[jsonvalue->value.array.size++] = *child_jsonvalue;
}


void spj_jsonvalue_object_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->value.object.size < *capacity) {
        SpjJSONNamedValue *data = realloc(jsonvalue->value.object.data, jsonvalue->value.object.size * sizeof(SpjJSONNamedValue));
        jsonvalue->value.object.data = data;
    }
}


void spj_jsonvalue_array_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->value.array.size < *capacity) {
        SpjJSONValue *data = realloc(jsonvalue->value.array.data, jsonvalue->value.array.size * sizeof(SpjJSONValue));
        jsonvalue->value.array.data = data;
    }
}


void spj_jsonvalue_enumerate(SpjJSONValue *jsonvalue, int (*function)(SpjJSONValue *jsonvalue)) {
    function(jsonvalue);

    switch (jsonvalue->type) {
        case SpjJSONValueArray: {
            size_t i;

            for (i = 0; i < jsonvalue->value.array.size; i++) {
                spj_jsonvalue_enumerate(& jsonvalue->value.array.data[i], function);
            }

            break;
        }

        case SpjJSONValueObject: {
            size_t i;

            for (i = 0; i < jsonvalue->value.object.size; i++) {
                spj_jsonvalue_enumerate(& jsonvalue->value.object.data[i].value, function);
            }

            break;
        }
            
        default:
            /* Do not "enumerate" leaves */

            break;
    }
}

void spj_jsonvalue_enumerate_reverse(SpjJSONValue *jsonvalue, int (*function)(SpjJSONValue *jsonvalue)) {
    size_t i;

    switch (jsonvalue->type) {
        case SpjJSONValueArray:
            for (i = 0; i < jsonvalue->value.array.size; i++) {
                spj_jsonvalue_enumerate_reverse(& jsonvalue->value.array.data[i], function);
            }

            break;

        case SpjJSONValueObject:
            for (i = 0; i < jsonvalue->value.object.size; i++) {
                spj_jsonvalue_enumerate_reverse(& jsonvalue->value.object.data[i].value, function);
            }

            break;

        default:
            break;
    }

    function(jsonvalue);
}


void spj_jsonvalue_free(SpjJSONValue *jsonvalue) {
    size_t i;

    if (jsonvalue == NULL) {
        return;
    }

    switch (jsonvalue->type) {
        case SpjJSONValueObject: {
            for (i = 0; i < jsonvalue->value.object.size; i++) {
                spj_jsonvalue_free(& jsonvalue->value.object.data[i].value);

                free(jsonvalue->value.object.data[i].name.data);

                jsonvalue->value.object.data[i].name = SpjStringZero; /* нужно? */
            }

            free(jsonvalue->value.object.data);

            break;
        }

        case SpjJSONValueArray: {
            for (i = 0; i < jsonvalue->value.array.size; i++) {
                spj_jsonvalue_free(&jsonvalue->value.array.data[i]);
            }

            free(jsonvalue->value.array.data);

            break;
        }

        case SpjJSONValueString:
            free(jsonvalue->value.string.data);

            break;

        default:
            break;
    }

    spj_jsonvalue_init(jsonvalue, jsonvalue->type);
}

