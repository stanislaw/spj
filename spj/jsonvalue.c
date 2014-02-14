

#include "jsonvalue.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void spj_jsonvalue_init(spj_jsonvalue_t *jsonvalue, spj_jsonvalue_type_t type) {
    jsonvalue->type = type;

    switch (type) {
        case SpjValueObject:
            jsonvalue->value.object = SpjObjectZero;

            break;

        case SpjValueArray:
            jsonvalue->value.array = SpjArrayZero;

            break;

        case SpjValueString:
            jsonvalue->value.string = SpjStringZero;

            break;

        case SpjValueNumber:
            jsonvalue->value.number = 0;

            break;

        default:
            break;
    }
}


void spj_jsonvalue_free(spj_jsonvalue_t *jsonvalue) {
    if (jsonvalue == NULL) {
        return;
    }

    switch (jsonvalue->type) {
        case SpjValueObject: {
            size_t i;

            for (i = 0; i < jsonvalue->value.object.size; i++) {
                free(jsonvalue->value.object.keys[i].data);
                jsonvalue->value.object.keys[i] = SpjStringZero;

                spj_jsonvalue_free(& jsonvalue->value.object.values[i]);
            }

            free(jsonvalue->value.object.values);

            break;
        }

        case SpjValueArray: {
            size_t i;
            
            for (i = 0; i < jsonvalue->value.array.size; i++) {
                spj_jsonvalue_free(&jsonvalue->value.array.values[i]);
            }

            free(jsonvalue->value.array.values);

            break;
        }

        case SpjValueString:
            free(jsonvalue->value.string.data);

            break;

        default:
            break;
    }

    memset(jsonvalue, 0, sizeof(spj_jsonvalue_t));
}

