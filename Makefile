CC=gcc
CFLAGS=-g -Wall -ansi -pedantic
LDFLAGS=
SOURCES=main.c
OUT=-o main

INCLUDE_PATHS:= -Ispj

run: all exec

# $(CC) $(CFLAGS) $(INCLUDE_PATHS) $(SOURCES) $(OUT)
all:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c spj/spj.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o main main.c spj.o	

exec:
	./main
