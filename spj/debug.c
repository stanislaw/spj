
#include "debug.h"
#include <assert.h>

void spj_jsonvalue_debug(SpjJSONValue *jsonvalue) {
    size_t i;

    printf("[Debug] ");

    switch (jsonvalue->type) {
        case SpjJSONValueArray:
            printf("[");

            for (i = 0; i < jsonvalue->array.size; i++) {
                SpjJSONValue data = jsonvalue->array.data[i];

                spj_jsonvalue_debug(&data);
            }

            printf("]");

            break;

        case SpjJSONValueObject:
            printf("{");

            for (i = 0; i < jsonvalue->object.size; i++) {
                SpjJSONNamedValue data = jsonvalue->object.data[i];

                printf("%s:-----", data.name.data);

                // TODO TODO TODO
                assert(0);
                //spj_jsonvalue_debug(&data);
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
    
    printf("\n");
}

