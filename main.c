#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spj/spj.h"


int main() {
    spj_error_t error;
    char *jsonbytes;
    size_t datasize;
    spj_jsonvalue_t jsonvalue;

    int N, i, j;
    double initial_time;


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
             }, \
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
            }, \
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
            }, \
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
            }, \
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
            }, \
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
            }, \
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
            }, \
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
            }, \
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
            }, \
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


    N = 10000;

    for (j = 0; j < 5; j++) {
        initial_time = get_time();

        datasize = strlen(jsonbytes);

        for (i = 0; i < N; i++) {

            spj_parse(jsonbytes, datasize, &jsonvalue, &error);
        }

        printf("time elapsed: %f seconds\n", get_time() - initial_time);
    }

    spj_jsonvalue_free(&jsonvalue);

    /*spj_jsonvalue_debug(&jsonvalue);*/

    /*printf("\n");*/

    return 0;
}
