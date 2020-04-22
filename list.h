#ifndef VIOLET_LIST_H
#define VIOLET_LIST_H

#ifdef LIST_STATIC
#define LSTDEF static
#else
#define LSTDEF
#endif

#if defined LIST_STATIC && !defined LIST_IMPLEMENTATION
#define LIST_IMPLEMENTATION
#endif

typedef struct list
{
	size_t sz;
	void *head, *tail;
	allocator_t *allocator;
} list_t;

typedef struct list__item_header
{
	void *prev, *next;
} list__item_header_t;

#define list(type) list_t

#define list_create()             list_create_ex(g_allocator)
#define list_create_ex(a)         (list_t){ .allocator = a }
#define list__allocator(l)        (l).allocator
#define list_sz(l)                (l).sz
#define list_empty(l)             (list_sz(l) == 0)
#define list_first(l)             (l).head
#define list_last(l)              (l).tail
#define list__item_header(i)      (((list__item_header_t*)(i))[-1])
#define list_prev(i)              list__item_header(i).prev
#define list_next(i)              list__item_header(i).next
#define list_append(l, i)         list__append(&(l), &(i), sizeof(i)  MEMCALL_LOCATION)
#define list_pop(l)               list__pop(&(l)  MEMCALL_LOCATION)
#define list_foreach(l, type, i)  for (type *i = (l).head; i != NULL; \
                                       i = list_next(i))
#define list_destroy(l)           list__destroy(&(l)  MEMCALL_LOCATION)


LSTDEF void *list__append(list_t *list, const void *item_, size_t sz  MEMCALL_ARGS);
LSTDEF void  list__pop(list_t *list  MEMCALL_ARGS);
LSTDEF void  list__destroy(list_t *list  MEMCALL_ARGS);

#endif // VIOLET_LIST_H

#ifdef LIST_IMPLEMENTATION

LSTDEF void *list__append(list_t *list, const void *item_, size_t sz  MEMCALL_ARGS)
{
	const size_t item_sz = sz + sizeof(list__item_header_t);
	allocator_t *a = list__allocator(*list);
	list__item_header_t *header = a->malloc_(item_sz, a  MEMCALL_VARS);
	void *item = header + 1;
	if (!header) { fatal("list__append: oom"); return NULL; }
	memset(header, 0, sizeof(list__item_header_t));
	memcpy(item, item_, sz);
	if (!list->head) {
		list->head = list->tail = item;
		header->prev = header->next = NULL;
	} else {
		list_next(list->tail) = item;
		header->prev = list->tail;
		list->tail = item;
	}
	++list->sz;
	return item;
}

LSTDEF void list__pop(list_t *list  MEMCALL_ARGS)
{
	list__item_header_t *tail_header;
	allocator_t *a = list__allocator(*list);
	assert(list->tail);
	tail_header = &list__item_header(list->tail);
	list->tail = tail_header->prev;
	if (list->tail)
		list__item_header(list->tail).next = NULL;
	else
		list->head = NULL;
	a->free_(tail_header, a  MEMCALL_VARS);
	--list->sz;
}

LSTDEF void list__destroy(list_t *list  MEMCALL_ARGS)
{
	while (!list_empty(*list))
		list__pop(list  MEMCALL_VARS);
}

#undef LIST_IMPLEMENTATION
#endif // LIST_IMPLEMENTATION
