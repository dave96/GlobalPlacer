#include <assert.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "draw.h"
#include "queue.h"

#define MAX_ITS 200
#define TOLERANCE (1e-7)

inline void initialize_eigen(double * u, int count) {
	double t = 0;

	for (int i = 0; i < count; ++i) {
		u[i] = (double)rand() / (double)((unsigned)RAND_MAX + 1);
		t += u[i] * u[i];
	}

	t = sqrt(t);

	for (int i = 0; i < count; ++i)
		u[i] /= t;
}

inline double inner_term(graph_t *g, double *u_k, int node) {
	// Sort of a dot product
	int *adj_i = &(g->laplacian[node * g->vertices]);
	double deg = adj_i[node];
	assert(deg != -1);

	double res = 0;
	for (int i = 0; i < g->vertices; ++i) {
		if (i != node)
			res += abs(adj_i[i]) * u_k[i];
	}

	return res / deg;
}

inline double dot_product(double *a, double *b, int n) {
	double r = 0;
	for (int i = 0; i < n; ++i)
		r += a[i] * b[i];
	return r;
}

// The actual first drawing is produced by an spectral layout,
// which is extracted from Y. Koren's article named
// Drawing Graphs by Eigenvectors: Theory and Practice.
//
// It uses a force method to minimize edge length while distributing
// nodes accross all dimensions uniformly. It also accounts
// for all dimensions to provide an equal amount of information (placement
// on each dimension cannot be correlated).
//
// The article describes the method in depth and it is the base for most
// spectral drawing code online. Additionally, we're using the laplacian
// instead of the adjacency matrix in this case.

placement_t spectral_draw(graph_t *g, int dims) {
	srand(time(NULL));
	double *u = malloc(g->vertices * (dims + 1) * sizeof(double));
	double * u_k = malloc(g->vertices * sizeof(double));

	// In general the first eigenvalue vector is simply set by ones, normalized by the euclidean distance.
	// As such, the euclidean distance of all-1 vector is sqrt(length), and all elements are 1/sqrt(length).
	double val = 1 / sqrt(g->vertices);
	for (int i = 0; i < g->vertices; ++i)
		u[i] = val;

	#define UDIM(dim) (&u[dim * g->vertices])
	#define DEG(i) (g->laplacian[i * g->vertices + i])

	// From dimension 1 onwards
	for (int k = 1; k < dims + 1; ++k) {
		double * uh_k = UDIM(k);
		initialize_eigen(uh_k, g->vertices);

		// Each dimension has to converge
		int iterations = 0;
		double residual = TOLERANCE + 1;
		while (iterations < MAX_ITS && residual > TOLERANCE)  {
			memcpy(u_k, uh_k, g->vertices * sizeof(double));

			// D-Orthogonalize against previous eigenvectors
			for (int l = 0; l < k; ++l) {
				double * u_l = UDIM(l);
				double upper = 0;
				double lower = 0;

				// Maybe this is not quite right? u_k and u_l are transposed
				// so there is a matrix result in the middle?
				for (int i = 0; i < g->vertices; ++i) {
					upper += u_k[i] * DEG(i) * u_l[i];
					lower += DEG(i) * u_l[i] * u_l[i];
				}

				double factor = upper / lower;

				for (int i = 0; i < g->vertices; ++i)
					u_k[i] -= factor * u_l[i];
			}

			// Multiply with 1/2 (I + D^-1A)
			// We have the Laplacian, so it's a bit different
			double euclid = 0;
			for (int i = 0; i < g->vertices; ++i) {
				uh_k[i] = 0.5 * (u_k[i] + (inner_term(g, u_k, i)));
				euclid += uh_k[i] * uh_k[i];
			}

			euclid = sqrt(euclid);
			// Normalize
			for (int i = 0; i < g->vertices; ++i)
				uh_k[i] /= euclid;

			iterations++;
			residual = 1 - dot_product(uh_k, u_k, g->vertices);
		}
	}

	// Transform the info we have into a placement
	placement_t res;
	res.dims = dims;
	res.vertices = g->vertices;
	res.coords = malloc(res.vertices * res.dims * sizeof(double));
	memcpy(res.coords, UDIM(1), res.vertices * res.dims * sizeof(double));

	#undef UDIM
	#undef DEG

	free(u_k);
	free(u);

	return res;
}

void bfs_stencil(int *stencil, int rows, int cols, int *x, int *y) {
	// Simple bfs inside a matrix: we look at all neighbors at certain distances.
	// Only that "simple" in C does not exist, of course.

	struct pos {
		int x, y;
	};

	int *visited = calloc(rows * cols, sizeof(int));
	queue_t q;
	q_initialize(&q, rows * cols, sizeof(struct pos));

	#define VISITED(x, y) visited[(x) * cols + (y)]

	struct pos initial = { *x, *y };
	q_push(&q, &initial);
	VISITED(*x, *y) = 1;

	while (!q_empty(&q)) {
		struct pos elem;
		q_pop(&q, &elem);

		if (stencil[elem.x * cols + elem.y] == 0) {
			*x = elem.x;
			*y = elem.y;
			break;
		}

		struct pos elemq;

		if (elem.x + 1 < rows && !VISITED(elem.x + 1, elem.y)) {
			elemq.x = elem.x + 1;
			elemq.y = elem.y;
			q_push(&q, &elemq);
			VISITED(elem.x + 1, elem.y) = 1;
		}

		if (elem.x - 1 > 0 && !VISITED(elem.x - 1, elem.y)) {
			elemq.x = elem.x - 1;
			elemq.y = elem.y;
			q_push(&q, &elemq);
			VISITED(elem.x - 1, elem.y) = 1;
		}

		if (elem.y + 1 < cols && !VISITED(elem.x, elem.y + 1)) {
			elemq.x = elem.x;
			elemq.y = elem.y + 1;
			q_push(&q, &elemq);
			VISITED(elem.x, elem.y + 1) = 1;
		}

		if (elem.y - 1 > 0 && !VISITED(elem.x, elem.y - 1)) {
			elemq.x = elem.x;
			elemq.y = elem.y - 1;
			q_push(&q, &elemq);
			VISITED(elem.x, elem.y - 1) = 1;
		}
	}

	#undef VISITED

	q_destroy(&q);
}

// Discretize the placement done by the spectral method to our given stencil
void discretize_spectral(placement_t *p, int rows, int cols) {
	// The output of the spectral methods goes from [-1, 1].
	// While we could just normalize based on minimums and maximums, we're just
	// artificially separating the nodes which is not necessarily what we want.

	int *stencil = malloc(rows * cols * sizeof(int));
	memset(stencil, 0, rows * cols * sizeof(int));

	for (int i = 0; i < p->vertices; ++i) {
		// Scale the coordinates
		double x = ((p->coords[i] + 1)/2) * (rows - 1);
		double y = ((p->coords[i + p->vertices] + 1)/2) * (cols - 1);

		// Round
		int x_d = (int) round(x);
		int y_d = (int) round(y);
		assert(x_d < rows);
		assert(y_d < cols);

		if (stencil[x_d * rows + y_d]) {
			// Occupied slot, we have to find another one
			bfs_stencil(stencil, rows, cols, &x_d, &y_d);
		}

		stencil[x_d * cols + y_d] = 1;
		p->coords[i] = x_d;
		p->coords[i + p->vertices] = y_d;
	}

	free(stencil);
}