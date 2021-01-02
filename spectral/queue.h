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

inline void q_initialize(queue_t *q, int capacity, int elem_size) {
	q->capacity = capacity + 1;
	q->elem_size = elem_size;
	q->head = 0;
	q->tail = 0;
	q->storage = malloc(capacity * elem_size);
}

inline int q_empty(queue_t *q) {
	return q->head == q->tail;
}

inline void q_pop(queue_t *q, void *location) {
	assert(!q_empty(q));
	memcpy(location, &(((char *)q->storage)[q->head * q->elem_size]), q->elem_size);
	q->head = (q->head + 1) % q->capacity;
}

inline void q_push(queue_t *q, void *location) {
	memcpy(location, &(((char *)q->storage)[q->tail * q->elem_size]), q->elem_size);
	q->tail = (q->tail + 1) % q->capacity;
	assert(!q_empty(q));
}

inline void q_destroy(queue_t *q) {
	q->capacity = 0;
	free(q->storage);
}

#endif // QUEUE_H