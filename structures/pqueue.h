#ifndef VIOLET_PQUEUE_H
#define VIOLET_PQUEUE_H

#include "violet/structures/array.h"

typedef struct pqueue
{
	array data;
	b8 (*cmp)(void*,void*);
} pqueue;

void pqueue_init(pqueue * q, u32 elem_size, b8 (*cmp)(void*,void*));
void pqueue_destroy(pqueue * q);
void pqueue_add(pqueue * q, void * elem);
b8 pqueue_empty(pqueue * q);
void * pqueue_next(pqueue * q);
void pqueue_pop(pqueue * q);

#endif
