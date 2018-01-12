CFLAGS+=-Wall -Wextra -Wpedantic
CFLAGS+=-Wwrite-strings -Wfloat-equal
CFLAGS+=-Waggregate-return -Winline 

LDFLAGS+=-lgmp

all: conversion fdr

conversion: conversions.c
	gcc  $(CFLAGS) -c -o conversions.o  conversions.c 

fdr: udp_server.c
	gcc $(CFLAGS) udp_server.c conversions.o -o fdr $(LDFLAGS)


clean:
	rm fdr *.o
