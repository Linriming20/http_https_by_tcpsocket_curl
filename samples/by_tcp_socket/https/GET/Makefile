all: a1.out a2.out

CC := gcc
CFLAGS := -I../include
LDFLAGS := -lmbedx509 -lmbedtls -lmbedcrypto -L../lib

a1.out : sample1.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

a2.out : sample2_download.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f a1.out a2.out
.PHONY := clean
