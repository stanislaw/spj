CC=clang

# -arch x86_64
CFLAGS=-O3 -g -Wall -Wextra -ansi -pedantic
LDFLAGS=
SOURCES=main.c
OUT=-o main

INCLUDE_PATHS:= -Ispj

run: o lib main exec

o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/spj.c -o build/spj.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/debug.c -o build/debug.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/lexer.c -o build/lexer.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/jsonvalue.c -o build/jsonvalue.o

lib:
	ar rs build/libspj.a build/*.o

main:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o main -L./build -lspj main.c

exec:
	./main

test: o lib _test runtests

_test:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o test -L./build -lspj test.c

runtests:
	./test

.PHONY: o lib main exec test _test runtests

