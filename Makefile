CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
SOURCES=main.m
OUT=-o main

all:
	$(CC) $(CFLAGS) $(SOURCES) $(OUT)

