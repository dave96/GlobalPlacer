#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graph.h"
#include "spectral/draw.h"
#include "anneal.h"

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

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < graph.vertices; ++j) {
			printf("%.2f ", place.coords[i * graph.vertices + j]);
		}
		printf("\n");
	}

	// Configure simmulated annealing
	sa_params_t params;
	// Changes per temperature step
	params.kmax = 50;
	// Initial temperature
	params.initial_temperature = 100;
	// Final temperature
	params.minimum_temperature = 0.001;
	// Temperature gradient
	params.alpha = 0.99;

	simulated_annealing(&graph, &place, rows, cols, params);

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < graph.vertices; ++j) {
			printf("%.2f ", place.coords[i * graph.vertices + j]);
		}
		printf("\n");
	}

	free_placement(&place);
	free_graph(&graph);

	return 0;
}
