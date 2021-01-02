#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>

#include "../graph.h"

typedef struct placement {
	int dims;
	int vertices;
	double *coords;
} placement_t;

placement_t spectral_draw(graph_t *g, int dims);

void discretize_spectral(placement_t *p, int rows, int cols);

inline void free_placement(placement_t *p) {
	free(p->coords);
}

#endif // DRAW_H