#include <string.h>
#include <stdlib.h>
#include "graph.h"

void read_graph(FILE *fp, graph_t *g)
{
	// Parse the TGF (Trivial Graph Format).
	int mode = 0; // Read nodes
	char str[200];
	g->vertices = 0;
	g->laplacian = NULL;

	while(fgets(str, 200, fp)) {
		if (!strcmp(str, "#\n")) {
			// Change mode
			mode = 1;
		} else {
			if (mode == 0) {
				// New vertex.
				g->vertices++;
			} else {
				// New edge
				if (!g->laplacian) {
					// First time we register an edge
					g->laplacian = calloc(g->vertices * g->vertices, sizeof(int));
				}

				int origin, destination;
				sscanf(str, "%d %d", &origin, &destination);
				// Our graphs start by index 1
				origin--; destination--;

				// Update the degree of both edges
				g->laplacian[g->vertices * origin + origin]++;
				g->laplacian[g->vertices * destination + destination]++;

				// Place a -1 on each edge's adjacency location
				g->laplacian[g->vertices * origin + destination] = -1;
				g->laplacian[g->vertices * destination + origin] = -1;
			}
		}
	}
}

void print_laplacian(graph_t *g)
{
	for (int i = 0; i < g->vertices; ++i) {
		for (int j = 0; j < g->vertices; ++j)
			printf("%d ", g->laplacian[g->vertices*i + j]);
		printf("\n");
	}
	printf("\n");
}

void free_graph(graph_t *g)
{
	free(g->laplacian);
}

