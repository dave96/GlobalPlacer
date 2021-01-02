all: placer

placer: spectral/draw.o graph.o main.o anneal.o
	gcc -O2 -lm -g -o $@ $^

%.o: %.c
	gcc -O2 -g -Wall -Wpedantic -c -o $@ $^

clean:
	rm -rf *.o placer
