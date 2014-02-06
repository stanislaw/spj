#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spj/spj.h"

#include <sys/time.h>
#include <sys/resource.h>

double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}


int main() {
    spj_error_t error;
    SpjJSONValue jsonvalue;
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


    int N = 100;

    double initial_time = get_time();

    for (int i = 0; i < N; i++) {
        spj_parse(jsonbytes, &jsonvalue, &error);
    }

    printf("time elapsed: %f seconds\n", get_time() - initial_time);



    //spj_jsonvalue_debug(&jsonvalue);

    spj_jsonvalue_free(&jsonvalue);
    
    //printf("\n");

    return 0;
}
