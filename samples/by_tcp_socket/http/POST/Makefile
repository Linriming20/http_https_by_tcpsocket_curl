all: a.out

CC := gcc
CFLAGS :=
LDFLAGS :=

a.out : sample.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f a.out
.PHONY := clean
