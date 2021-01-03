CFLAGS=-O3 -g -Wall -Wpedantic `pkg-config --cflags cairo x11` -fopenmp
CFLAGS_DEBUG=-O0 -g -Wall -Wpedantic -fsanitize=address `pkg-config --cflags cairo x11`
LIBS=-lm `pkg-config --libs cairo x11`
CC=gcc

all: placer

placer: spectral/draw.o spectral/queue.o graph.o main.o anneal.o cairo/visuals.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

placer_dbg: spectral/draw.o spectral/queue.o graph.o main.o anneal.o cairo/visuals.o
	$(CC) $(CFLAGS_DEBUG) $(LIBS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.o placer spectral/*.o cairo/*.o
