#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graph.h"

void usage(const char *name) {
	printf("Usage: %s <rows> <cols> <file>\n", name);
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		usage(argv[0]);
		return 1;
	}

	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);

	if (rows <= 0 || cols <= 0) {
		printf("Rows and cols must be positive and non-zero\n");
		return 1;
	}

	FILE *fp = fopen(argv[3], "r");
	if (fp == NULL) {
		printf("Cannot open graph file\n");
		return 1;
	}

	graph_t graph;
	read_graph(fp, &graph);
	fclose(fp);

	print_laplacian(&graph);
	free_graph(&graph);

	return 0;
}
