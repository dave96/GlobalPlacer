#include "queue.h"

void q_initialize(queue_t *q, int capacity, int elem_size) {
	q->capacity = capacity + 1;
	q->elem_size = elem_size;
	q->head = 0;
	q->tail = 0;
	q->storage = malloc(capacity * elem_size);
}

int q_empty(queue_t *q) {
	return q->head == q->tail;
}

void q_pop(queue_t *q, void *location) {
	assert(!q_empty(q));
	memcpy(location, &(((char *)q->storage)[q->head * q->elem_size]), q->elem_size);
	q->head = (q->head + 1) % q->capacity;
}

void q_push(queue_t *q, void *location) {
	memcpy(&(((char *)q->storage)[q->tail * q->elem_size]), location, q->elem_size);
	q->tail = (q->tail + 1) % q->capacity;
	assert(!q_empty(q));
}

void q_destroy(queue_t *q) {
	q->capacity = 0;
	free(q->storage);
}