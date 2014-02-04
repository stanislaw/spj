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
    SpjJSONData jsondata;
    spj_error_t error;

    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "{}";
    spj_parse(jsonbytes, &jsondata, &error);
    check(jsondata.type == SpjJSONValueObject);
    check(jsondata.value.object.size == 0);

    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[]";
    spj_parse(jsonbytes, &jsondata, &error);
    check(jsondata.type == SpjJSONValueArray);
    check(jsondata.value.array.size == 0);

    return 0;
}


int test_simple_root_objects() {
    char *jsonbytes;
    SpjJSONData jsondata;
    spj_error_t error;

    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[1]";
    spj_parse(jsonbytes, &jsondata, &error);
    check(jsondata.type == SpjJSONValueArray);

    check(jsondata.value.array.size == 1);
    check(error.message == NULL);


    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[1, 2, 3, 4]";
    spj_parse(jsonbytes, &jsondata, &error);
    check(jsondata.type == SpjJSONValueArray);

    check(jsondata.value.array.size == 4);
    check(error.message == NULL);


    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "{\"\":\"\", \"\": 1}";
    spj_parse(jsonbytes, &jsondata, &error);
    check(jsondata.type == SpjJSONValueObject);

    check(jsondata.value.object.size == 2);


    return 0;
}


int test_wrong_root_object() {
    char *jsonbytes;
    SpjJSONData jsondata;
    spj_error_t error;
    spj_result_t result;

    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    check(error.message == NULL);
    jsonbytes = "";
    result = spj_parse(jsonbytes, &jsondata, &error);
    check(error.message != NULL);
    check(result == SpjJSONParsingResultError);

    memset(&jsondata, 0, sizeof(SpjJSONData));
    memset(&error, 0, sizeof(spj_error_t));

    check(error.message == NULL);
    jsonbytes = "2";
    result = spj_parse(jsonbytes, &jsondata, &error);
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

