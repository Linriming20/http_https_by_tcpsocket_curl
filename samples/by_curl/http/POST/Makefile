all: a.out

CC := gcc
CFLAGS := -I../../include
LDFLAGS := -lcurl -lmbedx509 -lmbedtls -lmbedcrypto -L../../lib -L../../../by_tcp_socket/https/lib

LDFLAGS += -pthread -lz

a.out : sample.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f a.out
.PHONY := clean
