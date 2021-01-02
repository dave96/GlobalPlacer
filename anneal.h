#ifndef ANNEAL_H
#define ANNEAL_H

#include "graph.h"
#include "spectral/draw.h"

typedef struct sa_params {
	int kmax;
	double initial_temperature;
	double alpha;
} sa_params_t;

void simulated_annealing(graph_t *g, placement_t *p, int rows, int cols, sa_params_t params);

#endif // ANNEAL_H