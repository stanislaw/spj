CC=clang
CFLAGS=-g -Wall -Wextra -ansi -pedantic
LDFLAGS=
SOURCES=main.c
OUT=-o main

INCLUDE_PATHS:= -Ispj

run: all exec

all: o lib main

o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/spj.c -o spj.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/debug.c -o debug.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/lexer.c -o lexer.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/jsonvalue.c -o jsonvalue.o

lib:
	ar rs libspj.a *.o

main:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o main -L./ -lspj main.c

exec:
	./main

test: o lib _test runtests

_test:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o test -L./ -lspj test.c

runtests:
	./test

.PHONY: test run

