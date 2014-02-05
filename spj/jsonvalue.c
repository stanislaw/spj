

#import "jsonvalue.h"

#include <assert.h>
#include <stdlib.h>

void spj_jsonvalue_init(SpjJSONValue *jsonvalue, SpjJSONValueType type) {
    jsonvalue->type = type;

    jsonvalue->object.data = NULL;
    jsonvalue->object.size = 0;

    jsonvalue->array.data = NULL;
    jsonvalue->array.size = 0;

    jsonvalue->number = 0;
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

    for (size_t i = 0; i < jsonvalue->object.size; i++) {
//        if (jsonvalue->object.data[i].value.type == SpjJSONValueString) {
//            printf("string ----> %s\n", jsonvalue->object.data[i].value.string.data); // <- Segmentation fault: 11
//        }
    }
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
        SpjJSONNamedValue *data = realloc(jsonvalue->object.data, jsonvalue->object.size);
        jsonvalue->object.data = data;
    }
}


void spj_jsonvalue_array_finalize(SpjJSONValue *jsonvalue, size_t *capacity) {
    if (jsonvalue->array.size < *capacity) {
        SpjJSONValue *data = realloc(jsonvalue->array.data, jsonvalue->array.size);
        jsonvalue->array.data = data;
    }
}

