CFLAGS+=-Wall -Wextra -Wpedantic
CFLAGS+=-Wwrite-strings -Wfloat-equal
CFLAGS+=-Waggregate-return -Winline 

LDFLAGS+=-lgmp

all: fdr

fdr: udp_server.c
	gcc  $(CFLAGS) -c -o conversions.o  conversions.c
	gcc $(CFLAGS) udp_server.c conversions.o -o fdr $(LDFLAGS)

debug: 
	gcc  $(CFLAGS) -g -c -o conversions.o  conversions.c
	gcc $(CFLAGS) -g udp_server.c conversions.o -o fdr $(LDFLAGS)

profile:
	gcc  $(CFLAGS)-pg -c -o conversions.o  conversions.c
	gcc $(CFLAGS) -pg udp_server.c conversions.o -o fdr $(LDFLAGS)

clean:
	rm fdr *.o
