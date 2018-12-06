CC = gcc
OPTS = -g -Wall -W

all: chrash

chrash: chrash.c log.o
	$(CC) -o $@ $^ $(OPTS)

log.o: log.c log.h
	$(CC) -o $@ $< -c $(OPTS)

clean:
	rm -rf *.o shell
