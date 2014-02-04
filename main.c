#include <stdio.h>
#include <string.h>

#include "spj/spj.h"

int main() {
    spj_error_t error;
    SpjJSONData jsondata;
    char *jsonbytes;


    jsonbytes =
    "{ \
        \"data\": [ \
             { \
                 \"id\": 1, \
                 \"email\": \"john@example.com\", \
                 \"name\": \"John Smith\", \
                 \"nick\": \"John\", \
                 \"avatar\": \"https://d2cxspbh1aoie1.cloudfront.net/avatars/f0b4520a6e0001636bf8fc1431af151c/\", \
                 \"status\": \"Status message\", \
                 \"disabled\": false, \
                 \"last_activity\": 1366613085178, \
                 \"last_ping\": 1366616861547 \
             } \
        ] \
    }";



    printf("We can now iterate the whole string:\n");


    spj_parse(jsonbytes, &jsondata, &error);


    spj_jsondata_debug(&jsondata);
    
    
    printf("\n");

    return 0;
}
