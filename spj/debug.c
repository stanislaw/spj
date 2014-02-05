
#include "debug.h"
#include <assert.h>

static void spj_jsonvalue_debug_internal(SpjJSONValue *jsonvalue) {
    size_t i;

    switch (jsonvalue->type) {
        case SpjJSONValueArray:
            printf("[");

            for (i = 0; i < jsonvalue->array.size; i++) {
                if (i > 0) {
                    printf(", ");
                }

                SpjJSONValue data = jsonvalue->array.data[i];

                spj_jsonvalue_debug_internal(&data);
            }

            printf("]");

            break;

        case SpjJSONValueObject:
            printf("{");

            for (i = 0; i < jsonvalue->object.size; i++) {
                if (i > 0) {
                    printf(", ");
                }

                SpjJSONNamedValue data = jsonvalue->object.data[i];

                printf("\"");

                printf("%s", data.name.data);

                printf("\"");

                printf(":");

                spj_jsonvalue_debug_internal(&data);
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
    
}


void spj_jsonvalue_debug(SpjJSONValue *jsonvalue) {
    printf("[Debug] ");

    spj_jsonvalue_debug_internal(jsonvalue);

    printf("\n");

}