all: placer

placer: graph.o main.o
	gcc -O2 -g -o $@ $^

%.o: %.c
	gcc -O2 -g -Wall -Wpedantic -c -o $@ $^

clean:
	rm -rf *.o placer
