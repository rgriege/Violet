#include <assert.h>
#include <string.h>

#include "violet/structures/pqueue.h"

void pqueue_init(pqueue * q, u32 elem_size, b8 (*cmp)(void*,void*))
{
	array_init(&q->data, elem_size);
	q->cmp = cmp;
}

void pqueue_destroy(pqueue * q)
{
	array_destroy(&q->data);
}

void pqueue_add(pqueue * q, void * elem)
{
	s32 i = array_size(&q->data) - 1;
	for (; i >= 0; --i)
		if (q->cmp(elem, array_get(&q->data, i)))
			break;
	array_insert(&q->data, i + 1, elem);
}

b8 pqueue_empty(pqueue * q)
{
	return array_empty(&q->data);
}

void * pqueue_next(pqueue * q)
{
	assert(!pqueue_empty(q));
	return array_last(&q->data);
}

void pqueue_pop(pqueue * q)
{
	assert(!pqueue_empty(q));
	array_remove(&q->data, array_size(&q->data) - 1);
}
