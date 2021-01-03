#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graph.h"
#include "spectral/draw.h"
#include "anneal.h"
#include "cairo/visuals.h"

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

	if (graph.vertices > rows * cols) {
		printf("Placement impossible, not enough slots in the matrix\n");
		free_graph(&graph);
		return 1;
	}

	// print_laplacian(&graph);

	placement_t place = spectral_draw(&graph, 2);
	discretize_spectral(&place, rows, cols);
	// Now we have a first placement which is supposedly "good"

	// Configure simmulated annealing
	sa_params_t params;
	// Changes per temperature step
	params.kmax = graph.vertices * 6;
	// Initial temperature
	params.initial_temperature = graph.vertices * 2;
	// Final temperature
	params.minimum_temperature = 0.001;
	// Temperature gradient
	params.alpha = 0.98;

	visuals_render(&graph, &place, rows, cols);

	simulated_annealing(&graph, &place, rows, cols, params);

	visuals_render(&graph, &place, rows, cols);

	free_placement(&place);
	free_graph(&graph);

	return 0;
}
