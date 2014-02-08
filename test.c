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
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(jsonvalue.type == SpjJSONValueObject);
    check(jsonvalue.value.object.size == 0);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[]";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(jsonvalue.type == SpjJSONValueArray);
    check(jsonvalue.value.array.size == 0);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    return 0;
}


int test_simple_root_objects() {
    char *jsonbytes;
    SpjJSONValue jsonvalue;
    spj_error_t error;

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[1]";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);

    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    check(jsonvalue.type == SpjJSONValueArray);

    check(jsonvalue.value.array.size == 1);

    check(jsonvalue.value.array.data[0].type == SpjJSONValueNumber);
    check(jsonvalue.value.array.data[0].value.number == 1);


    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[1, 2, 3, 4]";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    check(jsonvalue.type == SpjJSONValueArray);
    check(jsonvalue.value.array.size == 4);

    check(jsonvalue.value.array.data[3].type == SpjJSONValueNumber);
    check(jsonvalue.value.array.data[3].value.number == 4);


    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "{\"hello\":\"world\", \"key\":\"value\", \"number\": \"one\"}";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    check(jsonvalue.type == SpjJSONValueObject);
    check(jsonvalue.value.object.size == 3);

    check(jsonvalue.value.object.data[0].value.type == SpjJSONValueString);

    check(jsonvalue.value.object.data[0].name.size == 5);
    check(jsonvalue.value.object.data[0].value.value.string.size == 5);

    check(strcmp(jsonvalue.value.object.data[0].name.data, "hello") == 0);
    check(strcmp(jsonvalue.value.object.data[0].value.value.string.data, "world") == 0);

    return 0;
}


int test_simple_root_objects_one_level_nesting() {
    char *jsonbytes;
    SpjJSONValue jsonvalue;
    spj_error_t error;


    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[{}]";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    check(jsonvalue.type == SpjJSONValueArray);

    check(jsonvalue.value.array.size == 1);

    check(jsonvalue.value.array.data[0].type == SpjJSONValueObject);
    check(jsonvalue.value.array.data[0].value.object.size == 0);
    check(jsonvalue.value.array.data[0].value.object.data == NULL);


    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "{\"array\":[1, 2, 3]}";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    check(jsonvalue.type == SpjJSONValueObject);

    check(jsonvalue.value.object.size == 1);

    check(strcmp(jsonvalue.value.object.data[0].name.data, "array") == 0);
    check(jsonvalue.value.object.data[0].value.type == SpjJSONValueArray);
    check(jsonvalue.value.object.data[0].value.value.array.size == 3);


    check(jsonvalue.value.object.data[0].value.value.array.data[0].type == SpjJSONValueNumber);
    check(jsonvalue.value.object.data[0].value.value.array.data[1].type == SpjJSONValueNumber);
    check(jsonvalue.value.object.data[0].value.value.array.data[2].type == SpjJSONValueNumber);

    check(jsonvalue.value.object.data[0].value.value.array.data[0].value.number == 1);
    check(jsonvalue.value.object.data[0].value.value.array.data[1].value.number == 2);
    check(jsonvalue.value.object.data[0].value.value.array.data[2].value.number == 3);


  return 0;
}


int test_simple_root_objects_two_level_nesting() {
    char *jsonbytes;
    SpjJSONValue jsonvalue;
    spj_error_t error;


    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    jsonbytes = "[[{\"hello\":\"world\"}]]";
    spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message == NULL);
    check(error.code == SpjJSONNoError);

    check(jsonvalue.type == SpjJSONValueArray);

    check(jsonvalue.value.array.size == 1);

    check(jsonvalue.value.array.data[0].type == SpjJSONValueArray);
    check(jsonvalue.value.array.data[0].value.array.size == 1);

    check(jsonvalue.value.array.data[0].value.array.data[0].value.object.data[0].name.data);
    check(strcmp(jsonvalue.value.array.data[0].value.array.data[0].value.object.data[0].name.data, "hello") == 0);
    check(strcmp(jsonvalue.value.array.data[0].value.array.data[0].value.object.data[0].value.value.string.data, "world") == 0);


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
    result = spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message != NULL);
    check(result == SpjJSONParsingResultError);

    memset(&jsonvalue, 0, sizeof(SpjJSONValue));
    memset(&error, 0, sizeof(spj_error_t));

    check(error.message == NULL);
    jsonbytes = "2";
    result = spj_parse(jsonbytes, strlen(jsonbytes), &jsonvalue, &error);
    check(error.message != NULL);
    check(result == SpjJSONParsingResultError);

    return 0;
}


int main() {
    test(test_empty, "general test for a empty JSON objects/arrays");
    test(test_simple_root_objects, "general test for JSON having correct simple objects/arrays");
    test(test_simple_root_objects_one_level_nesting, "general test for JSON having correct simple objects/arrays containing another objects/arrays (1 level of nesting)");
    test(test_simple_root_objects_one_level_nesting, "general test for JSON having correct simple objects/arrays containing another objects/arrays (2 level of nesting)");

    test(test_wrong_root_object, "test for JSON not having valid root objects (Object, Array");


    printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);

    return 0;
}



