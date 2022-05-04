all: a.out

CC := gcc
CFLAGS := -I../include
LDFLAGS := -lmbedx509 -lmbedtls -lmbedcrypto -L../lib

a.out : sample.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f a.out
.PHONY := clean
