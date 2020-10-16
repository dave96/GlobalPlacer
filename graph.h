#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

typedef struct graph {
	int vertices;
	int *laplacian;
} graph_t;

void read_graph(FILE *fp, graph_t *g);
void print_laplacian(graph_t *g);
void free_graph(graph_t *g);

#endif // GRAPH_H
