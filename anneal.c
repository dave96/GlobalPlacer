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

	#pragma omp parallel for schedule(static, 16) reduction(+: cost)
	for (int i = 0; i < g->vertices; ++i) {
		// For each vertex, calculate the manhattan distance to all adjacent nodes.
		// Only do so in the diagonal

		for (int j = i + 1; j < g->vertices; ++j) {
			cost += (abs(x[i] - x[j]) + abs(y[i] - y[j])) * abs(g->laplacian[i * g->vertices + j]);
		}
	}

	return cost;
}

double sa_estimate_diff(graph_t *g, int *x, int *y, int elem, int x_rand, int y_rand, int old_elem, int old_x, int old_y) {
	double cost = 0;
	// Calculate the cost of a but just looking at the nodes that have changed.
	// This is O(n) instead of O(n^2)

	for (int i = 0; i < g->vertices; ++i) {
		// Substract the old cost and add the new cost
		if (i == elem)
			continue;

		// If i == the old element there is no change in cost
		// If they did not have an edge, nothing changes
		// Otherwise, we just swap them, distance is the same
		if (i != old_elem) {
			cost -= (abs(x[i] - old_x) + abs(y[i] - old_y)) * abs(g->laplacian[elem * g->vertices + i]);
			cost += (abs(x[i] - x_rand) + abs(y[i] - y_rand)) * abs(g->laplacian[elem * g->vertices + i]);

			if (old_elem != -1) {
				cost -= (abs(x[i] - x_rand) + abs(y[i] - y_rand)) * abs(g->laplacian[old_elem * g->vertices + i]);
				cost += (abs(x[i] - old_x) + abs(y[i] - old_y)) * abs(g->laplacian[old_elem * g->vertices + i]);
			}
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

			double diff = sa_estimate_diff(g, x, y, elem, x_rand, y_rand, old_elem, old_x, old_y);

			if (diff < 0) {
				// Keep
				cost += diff;
			} else {
				// Between 0 and 1
				double r = (double)rand() / (double)(RAND_MAX);

				if (r < exp(-diff / temp)) {
					cost += diff;
				} else {
					// Do not keep
					continue;
				}
			}

			// Swap
			stencil[x_rand * cols + y_rand] = elem;
			x[elem] = x_rand;
			y[elem] = y_rand;

			stencil[old_x * cols + old_y] = old_elem;
			if (old_elem != -1) {
				x[old_elem] = old_x;
				y[old_elem] = old_y;
			}
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