CC=gcc
CFLAGS+=-Wall -Wextra -Wpedantic
CFLAGS+=-Wwrite-strings -Wfloat-equal
CFLAGS+=-Waggregate-return -Winline

all: conversion fdr

conversion: conversions.c
	$(CC) $(CFLAGS) -c conversions.c -o conversions.o

fdr: udp_server.c
	$(CC) $(CFLAGS) udp_server.c conversions.o -o fdr 


clean:
	rm fdr *.o
