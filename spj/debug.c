
#include "debug.h"
#include <assert.h>
#include "jsonvalue.h"

static void spj_jsonvalue_debug_internal(spj_jsonvalue_t *jsonvalue) {
    size_t i;

    switch (jsonvalue->type) {
        case SpjValueArray: {
            spj_jsonvalue_t value;

            printf("[");

            for (i = 0; i < jsonvalue->value.array.size; i++) {
                if (i > 0) {
                    printf(", ");
                }

                value = jsonvalue->value.array.values[i];

                spj_jsonvalue_debug_internal(&value);
            }

            printf("]");

            break;
        }

        case SpjValueObject: {
            spj_jsonvalue_t value;

            printf("{");

            for (i = 0; i < jsonvalue->value.object.size; i++) {
                if (i > 0) {
                    printf(", ");
                }

                value = jsonvalue->value.object.values[i];

                printf("\"");

                printf("%s", jsonvalue->value.object.keys[i].data);

                printf("\"");

                printf(":");

                spj_jsonvalue_debug_internal(&value);
            }

            printf("}");

            break;
        }

        case SpjValueString:
            printf("\"");

            printf("%s", jsonvalue->value.string.data);

            printf("\"");

            break;

        case SpjValueNumber:
            printf("%f", jsonvalue->value.number);

            break;

        case SpjValueNull:
            printf("null");

            break;

        case SpjValueTrue:
            printf("true");

            break;

        case SpjValueFalse:
            printf("false");

            break;

        default:

            assert(0);

            break;
    }
    
}


void spj_jsonvalue_debug(spj_jsonvalue_t *jsonvalue) {
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

