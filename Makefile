CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
SOURCES=main.m
OUT=-o main

INCLUDE_PATHS:= -Ispj

run: all exec

all:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(SOURCES) $(OUT)

exec:
	./main
