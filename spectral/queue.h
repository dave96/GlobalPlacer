#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct queue {
	int capacity;
	int elem_size;
	int head;
	int tail;
	void *storage;
} queue_t;

void q_initialize(queue_t *q, int capacity, int elem_size);

int q_empty(queue_t *q);

void q_pop(queue_t *q, void *location);

void q_push(queue_t *q, void *location);

void q_destroy(queue_t *q);

#endif // QUEUE_H