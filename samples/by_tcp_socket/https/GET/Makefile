all: a.out

CC := gcc
CFLAGS := -I../include
LDFLAGS := -lmbedx509 -lmbedtls -lmbedcrypto -L../lib

LDFLAGS += -pthread -lz

a.out : sample.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f a.out
.PHONY := clean
