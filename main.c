#include <stdio.h>
#include <string.h>

#include "spj.h"

int main() {
    char *jsonbytes = "{\"Here some\":\"JSON bytes\"}";
    int datasize = strlen(jsonbytes);

    printf("We can now iterate the whole string:\n");


    spj_parse(jsonbytes, datasize);

    printf("\n");

    return 0;
}
