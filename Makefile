
CFLAGS=-c -g -Wall -Werror -std=c99 -pedantic
LDFLAGS=-lssl -lcrypto
CC = gcc
LD = gcc -pthread
OBJS = dhtnode.o
PROG = dhtnode

.c.o:
	gcc $< -o $@ $(CFLAGS)

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(OBJS) -o $(PROG) $(LDFLAGS)

dhtnode.o: dhtnode.c dhtpackettypes.h
	$(CC) $(CFLAGS) -c dhtnode.c

clean:
	/bin/rm -f *.o dhtnode
