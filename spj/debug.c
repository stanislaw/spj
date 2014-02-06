
#include "debug.h"
#include <assert.h>
#include "jsonvalue.h"

static void spj_jsonvalue_debug_internal(SpjJSONValue *jsonvalue) {
    size_t i;

    switch (jsonvalue->type) {
        case SpjJSONValueArray: {
            SpjJSONValue data;

            printf("[");

            for (i = 0; i < jsonvalue->value.array.size; i++) {
                if (i > 0) {
                    printf(", ");
                }

                data = jsonvalue->value.array.data[i];

                spj_jsonvalue_debug_internal(&data);
            }

            printf("]");

            break;
        }

        case SpjJSONValueObject: {
            SpjJSONNamedValue data;

            printf("{");

            for (i = 0; i < jsonvalue->value.object.size; i++) {
                if (i > 0) {
                    printf(", ");
                }

                data = jsonvalue->value.object.data[i];

                printf("\"");

                printf("%s", data.name.data);

                printf("\"");

                printf(":");

                spj_jsonvalue_debug_internal(&data.value);
            }

            printf("}");

            break;
        }

        case SpjJSONValueString:
            printf("\"");

            printf("%s", jsonvalue->value.string.data);

            printf("\"");

            break;

        case SpjJSONValueNumber:
            printf("%f", jsonvalue->value.number);

            break;

        case SpjJSONValueNull:
            printf("null");

            break;

        case SpjJSONValueBool:
            if (jsonvalue->value.number == 1) {
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


double get_time() {
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

