CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
SOURCES=main.m
OUT=-o main

INCLUDE_PATHS:= -I$(dir $(lastword $(MAKEFILE_LIST)))/spj

all:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(SOURCES) $(OUT)

