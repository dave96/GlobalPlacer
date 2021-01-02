#include "anneal.h"
#include "cairo/visuals.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void build_stencil(graph_t *g, placement_t *p, int rows, int cols, int *stencil) {
	for (int i = 0; i < rows * cols; ++i)
		stencil[i] = -1;

	for (int i = 0; i < g->vertices; ++i) {
		int x = (int) p->coords[i];
		int y = (int) p->coords[i + g->vertices];
		stencil[x * cols + y] = i;
	}
}

double sa_cost(graph_t *g, int *x, int *y) {
	double cost = 0;
	// Calculate the cost of a solution.

	for (int i = 0; i < g->vertices; ++i) {
		// For each vertex, calculate the manhattan distance to all adjacent nodes.
		// Only do so in the diagonal

		for (int j = i + 1; j < g->vertices; ++j) {
			if (g->laplacian[i * g->vertices + j] != 0)
				cost += (abs(x[i] - x[j]) + abs(y[i] - y[j]));
		}
	}

	return cost;
}

char buffer[1024];

void simulated_annealing(graph_t *g, placement_t *p, int rows, int cols, sa_params_t params) {
	srand(time(NULL));
	double temp = params.initial_temperature;

	// Auxiliary structures
	int *stencil = malloc(rows * cols * sizeof(int));
	build_stencil(g, p, rows, cols, stencil);

	int *x = malloc(g->vertices * sizeof(int));
	int *y = malloc(g->vertices * sizeof(int));

	for (int i = 0; i < g->vertices; ++i) {
		x[i] = (int) p->coords[i];
		y[i] = (int) p->coords[i + g->vertices];
	}

	double cost = sa_cost(g, x, y);
	printf("Initial cost: %.2f\n", cost);

	int steps = 0;
	while(temp > params.minimum_temperature) {
		for (int k = 0; k < params.kmax; ++k) {
			// Select 1 element at random.
			int elem = rand() % g->vertices;
			// Then select one place on the grid to move it to
			int x_rand = rand() % rows;
			int y_rand = rand() % cols;

			// Save old elements
			int old_elem = stencil[x_rand * cols + y_rand];
			int old_x = x[elem];
			int old_y = y[elem];

			// Swap
			stencil[x_rand * cols + y_rand] = elem;
			x[elem] = x_rand;
			y[elem] = y_rand;

			stencil[old_x * cols + old_y] = old_elem;
			if (old_elem != -1) {
				x[old_elem] = old_x;
				y[old_elem] = old_y;
			}

			// Evaluate
			double newcost = sa_cost(g, x, y);
			double difference = newcost - cost;

			if (difference < 0) {
				// Keep
				cost = newcost;
				continue;
			} else {
				// Between 0 and 1
				double r = (double)rand() / (double)(RAND_MAX);

				if (r < exp(-difference / temp)) {
					cost = newcost;
					continue;
				}
			}

			// Reset
			stencil[old_x * cols + old_y] = elem;
			if (old_elem != -1) {
				x[old_elem] = x_rand;
				y[old_elem] = y_rand;
			}

			stencil[x_rand * cols + y_rand] = old_elem;
			x[elem] = old_x;
			y[elem] = old_y;
		}

		temp *= params.alpha;
		steps++;

		// if (steps % 10 == 0) {
		// 	// Copy to the actual placement
		// 	for (int i = 0; i < g->vertices; ++i) {
		// 		p->coords[i] = x[i];
		// 		p->coords[i + g->vertices] = y[i];
		// 	}

		// 	sprintf(buffer, "out/step_%05d.png", steps);
		// 	visuals_png(g, p, rows, cols, buffer);
		// }
	}

	printf("Final cost: %.2f\n", cost);
	printf("Steps: %d\n", steps);
	free(stencil);

	// Copy to the actual placement
	for (int i = 0; i < g->vertices; ++i) {
		p->coords[i] = x[i];
		p->coords[i + g->vertices] = y[i];
	}

	// visuals_png(g, p, rows, cols, "step_final.png");

	free(x);
	free(y);
}