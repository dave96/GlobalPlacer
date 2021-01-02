#ifndef VISUALS_H
#define VISUALS_H

#include "../graph.h"
#include "../spectral/draw.h"

void visuals_render(graph_t *graph, placement_t *place, int rows, int cols);

void visuals_png(graph_t *graph, placement_t *place, int rows, int cols, const char *name);

#endif // VISUALS_H