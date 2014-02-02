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

    jsonbytes = "{}";
    memset(&jsondata, 0, sizeof(SpjJSONData));
    spj_parse(jsonbytes, &jsondata);
    check(jsondata.type == SpjJSONValueObject);
    //check(jsondata.value.object.data == NULL);

    jsonbytes = "[]";
    memset(&jsondata, 0, sizeof(SpjJSONData));
    spj_parse(jsonbytes, &jsondata);
    check(jsondata.type == SpjJSONValueArray);
    //check(jsondata.value.array.data == NULL);

    return 0;
}

int main() {
    test(test_empty, "general test for a empty JSON objects/arrays");


    printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);

    return 0;
}
