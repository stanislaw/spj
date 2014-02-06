#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spj/spj.h"


static int test_passed = 0;
static int test_failed = 0;

/* Terminate current test with error */
#define fail()  return __LINE__

/* Successfull end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond) do { if (!(cond)) fail(); } while (0)


/* Test runner */
static void test(int (*func)(void), const char *name) {
  int r = func();

  if (r == 0) {
    test_passed++;
  } else {
    test_failed++;
    printf("FAILED: %s (at line %d)\n", name, r);
  }
}


int test_empty() {
    char *jsonbytes;
    SpjJSONValue jsonvalue;
    spj_error_t error;

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "{}";
    spj_parse(jsonbytes, &jsonvalue, &error);
    check(jsonvalue.type == SpjJSONValueObject);
    check(jsonvalue.value.object.size == 0);

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[]";
    spj_parse(jsonbytes, &jsonvalue, &error);
    check(jsonvalue.type == SpjJSONValueArray);
    check(jsonvalue.value.array.size == 0);

    return 0;
}


int test_simple_root_objects() {
    char *jsonbytes;
    SpjJSONValue jsonvalue;
    spj_error_t error;
    SpjJSONValue value;
    SpjJSONNamedValue namedvalue;

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[1]";
    spj_parse(jsonbytes, &jsonvalue, &error);
    check(error.message == NULL);

    check(jsonvalue.type == SpjJSONValueArray);

    check(jsonvalue.value.array.size == 1);

    value = jsonvalue.value.array.data[0];
    check(value.type == SpjJSONValueNumber);
    check(value.value.number == 1);



    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[1, 2, 3, 4]";
    spj_parse(jsonbytes, &jsonvalue, &error);
    check(error.message == NULL);

    check(jsonvalue.type == SpjJSONValueArray);
    check(jsonvalue.value.array.size == 4);

    value = jsonvalue.value.array.data[3];
    check(value.type == SpjJSONValueNumber);
    check(value.value.number == 4);


    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    printf("\nCRITICAL\n\n");

    jsonbytes = "{\"hello\":\"world\", \"key\":\"value\", \"number\": \"one\"}";
    spj_parse(jsonbytes, &jsonvalue, &error);
    check(error.message == NULL);

    check(jsonvalue.type == SpjJSONValueObject);
    check(jsonvalue.value.object.size == 3);

    namedvalue = jsonvalue.value.object.data[0];
    check(namedvalue.value.type == SpjJSONValueString);

    check(namedvalue.name.size == 5);
    check(namedvalue.value.value.string.size == 5);

    check(strcmp(namedvalue.name.data, "hello") == 0);
    check(strcmp(namedvalue.value.value.string.data, "world") == 0);

    return 0;
}


int test_wrong_root_object() {
    char *jsonbytes;
    SpjJSONValue jsonvalue;
    spj_error_t error;
    spj_result_t result;

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    check(error.message == NULL);
    jsonbytes = "";
    result = spj_parse(jsonbytes, &jsonvalue, &error);
    check(error.message != NULL);
    check(result == SpjJSONParsingResultError);

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    check(error.message == NULL);
    jsonbytes = "2";
    result = spj_parse(jsonbytes, &jsonvalue, &error);
    check(error.message != NULL);
    check(result == SpjJSONParsingResultError);

    return 0;
}


int main() {
    test(test_empty, "general test for a empty JSON objects/arrays");
    test(test_simple_root_objects, "general test for JSON having correct simple objects/arrays");


    test(test_wrong_root_object, "test for JSON not having valid root objects (Object, Array");


    printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);

    return 0;
}



