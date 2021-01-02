CFLAGS=-O3 -g -Wall -Wpedantic
CFLAGS_DEBUG=-O0 -g -Wall -Wpedantic -fsanitize=address
CC=gcc

all: placer

placer: spectral/draw.o spectral/queue.o graph.o main.o anneal.o
	$(CC) $(CFLAGS) -lm -o $@ $^

placer_dbg: spectral/draw.o spectral/queue.o graph.o main.o anneal.o
	$(CC) $(CFLAGS_DEBUG) -lm -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.o placer spectral/*.o
