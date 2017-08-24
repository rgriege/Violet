#ifndef VIOLET_LIST_H
#define VIOLET_LIST_H

#ifndef list_size_t
#define list_size_t size_t
#endif

#ifdef LIST_STATIC
#define LSTDEF static
#else
#define LSTDEF
#endif

#if defined LIST_STATIC && !defined LIST_IMPLEMENTATION
#define LIST_IMPLEMENTATION
#endif

#include <stdlib.h>
#include <string.h>

typedef struct list
{
	size_t sz;
	void *head, *tail;
} list_t;

typedef struct list__item_header
{
	void *prev, *next;
} list__item_header_t;

#define list(type) list_t

#define list_sz(l)                (l).sz
#define list_empty(l)             (list_sz(l) == 0)
#define list_first(l)             (l).head
#define list_last(l)              (l).tail
#define list__item_header(i)      (((list__item_header_t*)(i))[-1])
#define list_prev(i)              list__item_header(i).prev
#define list_next(i)              list__item_header(i).next
// #define list_next(i)              *(void**)(i + 1)
#define list_append(l, i)         list__append(&(l), &(i), sizeof(i))
#define list_pop(l)               list__pop(&(l))
#define list_foreach(l, type, i)  for (type *i = (l).head; i != NULL; \
                                       i = list_next(i))
#define list_destroy(l)           do { \
                                    list_foreach((l), void, list_iter_) \
                                      free(&list__item_header(list_iter_)); \
                                  } while (0);


LSTDEF void list__append(list_t *list, const void *item_, size_t sz);
LSTDEF void list__pop(list_t *list);

#endif // VIOLET_LIST_H

#ifdef LIST_IMPLEMENTATION

LSTDEF void list__append(list_t *list, const void *item_, size_t sz)
{
	list__item_header_t *header = malloc(sz + sizeof(list__item_header_t));
	void *item = header + 1;
	memset(header, 0, sizeof(list__item_header_t));
	memcpy(item, item_, sz);
	if (!list->head) {
		list->head = list->tail = item;
		header->prev = header->next = NULL;
	} else {
		list_next(list->tail) = item; // memcpy((char*)list->tail + sz, &item, sizeof(void*));
		header->prev = list->tail;
		list->tail = item;
	}
	++list->sz;
}

LSTDEF void list__pop(list_t *list)
{
	list__item_header_t *tail_header;
	assert(list->tail);
	tail_header = &list__item_header(list->tail);
	list->tail = tail_header->prev;
	if (list->tail)
		list__item_header(list->tail).next = NULL;
	else
		list->head = NULL;
	free(tail_header);
	--list->sz;
}

#undef LIST_IMPLEMENTATION
#endif // LIST_IMPLEMENTATION
