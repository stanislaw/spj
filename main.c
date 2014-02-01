#include <stdio.h>
#include <string.h>

#include "spj/spj.h"

int main() {
    char *jsonbytes = "{\"Here some\":\"JSON bytes\"}";
    SpjJSONData jsondata;

    printf("We can now iterate the whole string:\n");


    spj_parse(jsonbytes, &jsondata);

    printf("\n");

    return 0;
}
