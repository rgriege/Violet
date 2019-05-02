#ifndef PGB_H
#define PGB_H

/*
 * PaGed Bump allocator
 *
 * A bump allocator that divides memory into pages instead of a single
 * contiguous segment. Unused pages are stored in a heap, which can be
 * shared between multiple allocators. In addition to traditional memory
 * calls, a watermark can be saved & restored as an easy way to free many
 * allocations at once.
 */

#define PGB_MIN_PAGE_SIZE 4096

typedef struct pgb_heap
{
	struct pgb_page *first_page;
} pgb_heap_t;

void pgb_heap_init(pgb_heap_t *heap);
void pgb_heap_destroy(pgb_heap_t *heap);

struct pgb_page *pgb_heap_borrow_page(pgb_heap_t *heap, size_t min_size, size_t max_size);
void             pgb_heap_return_page(pgb_heap_t *heap, struct pgb_page *page);


typedef uint8_t pgb_byte;
typedef struct pgb
{
	struct pgb_heap *heap;
	struct pgb_page *current_page;
	pgb_byte *current_ptr;
} pgb_t;

void pgb_init(pgb_t *pgb, pgb_heap_t *heap);
void pgb_destroy(pgb_t *pgb);

void *pgb_malloc(size_t size, pgb_t *pgb  MEMCALL_ARGS);
void *pgb_calloc(size_t nmemb, size_t size, pgb_t *pgb  MEMCALL_ARGS);
void *pgb_realloc(void *ptr, size_t size, pgb_t *pgb  MEMCALL_ARGS);
void  pgb_free(void *ptr, pgb_t *pgb  MEMCALL_ARGS);

typedef struct pg_watermark
{
	struct pgb *pgb;
	struct pgb_page *page;
	pgb_byte *ptr;
} pgb_watermark_t;

pgb_watermark_t pgb_save(pgb_t *pgb);
void            pgb_restore(pgb_watermark_t watermark);

b32  pgb_owns_ptr(const pgb_t *pgb, const void *ptr);
void pgb_stats(const pgb_t *pgb, size_t *bytes_used, size_t *pages_used,
               size_t *bytes_available, size_t *pages_available);
void pgb_log(const pgb_t *pgb, const char *name);
void pgb_log_heap(const pgb_t *pgb);

#endif // PGB_H



/* Implementation */
#ifdef PGB_IMPLEMENTATION

/*
 * Always have 254-255 (256 - ceil(sizeof(header) / alignment)) slots:
 *   header size is constant -> more efficent at larger page sizes
 *   less efficient to allocate small objects into large pages (complex)
 *
 *   reserved slots = max(1, 256 / alignment) - max(1, sizeof(header) / alignment)
 *
 *   4K   ->   16 byte alignment -> 238 usable slots -> 93% efficient
 *   8K   ->   32 byte alignment -> 247 usable slots -> 96% efficient
 *   64K  ->  256 byte alignment -> 254 usable slots -> 99% efficient
 */


/* Page */

#define PGB__PAGE_SLOTS            256
#define pgb__alignment(page_size)  ((page_size) / PGB__PAGE_SLOTS)

typedef struct pgb_page
{
	union
	{
		pgb_byte start;
		pgb_byte alloc_sizes[PGB__PAGE_SLOTS];
	};
	union
	{
		pgb_byte header_begin;
		struct
		{
			struct pgb_page *prev, *next;
			size_t size;
			pgb_byte header_end;
		};
		pgb_byte first_slot;
	};
} pgb_page_t;


typedef union pgb__max_align
{
	long l;
	unsigned long ul;
	double d;
	long double ld;
	void *p;
	void (*f)(void);
} pgb__max_align_t;

static_assert(sizeof(pgb__max_align_t) <= pgb__alignment(PGB_MIN_PAGE_SIZE),
              invalid_word_size_for_pgb_allocator);

#define pgb__page_alignment(page)   (pgb__alignment((page)->size))
#define pgb__page_end(page)         (&(page)->start + (page)->size)
#define pgb__header_size_()         (  offsetof(pgb_page_t, header_end) \
                                     - offsetof(pgb_page_t, header_begin))
#define pgb__header_size(page_size) (pgb__align(pgb__header_size_(), \
                                                pgb__alignment(page_size)))

static
size_t pgb__align(size_t size, size_t alignment)
{
	return size + (alignment - (size % alignment)) % alignment;
}

static
size_t pgb__page_align(size_t size, const pgb_page_t *page)
{
	return pgb__align(size, pgb__page_alignment(page));
}

static
pgb_byte *pgb__page_first_usable_slot(pgb_page_t *page)
{
	return &page->first_slot + pgb__header_size(page->size);
}

static
pgb_byte *pgb__alloc_get_sz_slot(const pgb_byte* ptr, pgb_page_t *page)
{
	return &page->alloc_sizes[(ptr - &page->start) / pgb__page_alignment(page)];
}

static
size_t pgb__alloc_get_sz(const pgb_byte* ptr, const pgb_page_t *page)
{
	const size_t alignment = pgb__page_alignment(page);
	return page->alloc_sizes[(ptr - &page->start) / alignment] * alignment;
}

static
void pgb__alloc_set_sz(const pgb_byte *ptr, pgb_page_t *page, size_t sz)
{
	const size_t alignment = pgb__page_alignment(page);
	page->alloc_sizes[(ptr - &page->start) / alignment] = (pgb_byte)(sz / alignment);
}

static
b32 pgb__ptr_in_page(const pgb_byte *ptr, const pgb_page_t *page)
{
	return ptr > &page->start && ptr < pgb__page_end(page);
}

static
b32 pgb__ptr_is_last_alloc(const pgb_byte *ptr, const pgb_t *pgb)
{
	return   pgb__ptr_in_page(ptr, pgb->current_page)
	      && ptr + pgb__alloc_get_sz(ptr, pgb->current_page) == pgb->current_ptr;
}

static
size_t pgb__round_up_power_of_two(size_t x_)
{
	size_t x = x_ - 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
#if INTPTR_MAX == INT64_MAX
	x |= x >> 32;
#elif INTPTR_MAX != INT32_MAX
#error Unhandled size_t size
#endif
	return x + 1;
}

static
size_t pgb__page_min_size_for_alloc(size_t alloc_size)
{
	const size_t page_size = pgb__round_up_power_of_two(alloc_size);
	if (page_size < PGB_MIN_PAGE_SIZE)
		return PGB_MIN_PAGE_SIZE;
	else if (page_size - PGB__PAGE_SLOTS - pgb__header_size(page_size) < alloc_size)
		return page_size << 1;
	else
		return page_size;
}

static
size_t pgb__page_max_size_for_alloc(size_t alloc_size)
{
	/* Ensure maximum waste of 25% per slot.
	 * This is unlikely to occur as the smallest page that fits will be used. */
	const size_t max_alignment = pgb__round_up_power_of_two(alloc_size) << 1;
	const size_t max_page_size = max_alignment * PGB__PAGE_SLOTS;
	return max(max_page_size, PGB_MIN_PAGE_SIZE);
}

static
void pgb__page_clear(pgb_page_t *page)
{
	const size_t header_size = pgb__header_size(page->size);
	memset(page, 0, PGB__PAGE_SLOTS);
	pgb__alloc_set_sz(&page->header_begin, page, header_size);
	page->prev = NULL;
	page->next = NULL;
}

static
pgb_page_t *pgb__page_create(size_t page_size)
{
	pgb_page_t *page = std_malloc(page_size);
	page->size = page_size;
	pgb__page_clear(page);
	return page;
}

static
void pgb__page_remove(pgb_page_t *page)
{
	if (page->prev)
		page->prev->next = page->next;
	if (page->next)
		page->next->prev = page->prev;
}


/* Heap */

void pgb_heap_init(pgb_heap_t *heap)
{
	heap->first_page = NULL;
}

void pgb_heap_destroy(pgb_heap_t *heap)
{
	pgb_page_t *page = heap->first_page;
	while (page) {
		pgb_page_t *next = page->next;
		std_free(page);
		page = next;
	}
	heap->first_page = NULL;
}

struct pgb_page *pgb_heap_borrow_page(pgb_heap_t *heap, size_t min_size, size_t max_size)
{
	assert(min_size <= max_size);
	pgb_page_t *page = heap->first_page;
	while (page && page->size < min_size)
		page = page->next;
	if (!page || page->size > max_size) {
		return pgb__page_create(min_size);
	} else {
		if (page == heap->first_page)
			heap->first_page = page->next;
		pgb__page_remove(page);
		pgb__page_clear(page);
		return page;
	}
}

void pgb_heap_return_page(pgb_heap_t *heap, struct pgb_page *page)
{
	if (!heap->first_page) {
		heap->first_page = page;
		page->prev = NULL;
		page->next = NULL;
	} else if (page->size <= heap->first_page->size) {
		page->prev = NULL;
		page->next = heap->first_page;
		heap->first_page->prev = page;
		heap->first_page = page;
	} else {
		pgb_page_t *prev = NULL;
		pgb_page_t *next = heap->first_page;
		while (next && page->size > next->size) {
			prev = next;
			next = next->next;
		}
		if (prev)
			prev->next = page;
		if (next)
			next->prev = page;
		page->prev = prev;
		page->next = next;
	}
}

/* Allocator */

void pgb_init(pgb_t *pgb, pgb_heap_t *heap)
{
	pgb->heap         = heap;
	pgb->current_page = NULL;
	pgb->current_ptr  = NULL;
}

void pgb_destroy(pgb_t *pgb)
{
	if (pgb->current_page) {
		pgb_page_t *tail = pgb->current_page;
		assert(!tail->next);
		while (tail) {
			pgb_page_t *prev = tail->prev;
			pgb_heap_return_page(pgb->heap, tail);
			tail = prev;
		}
	}
}

static
void pgb__add_page_for_alloc(pgb_t *pgb, size_t alloc_size, size_t *aligned_size)
{
	const size_t min_page_size = pgb__page_min_size_for_alloc(alloc_size);
	const size_t max_page_size = pgb__page_max_size_for_alloc(alloc_size);
	pgb_page_t *page = pgb_heap_borrow_page(pgb->heap, min_page_size, max_page_size);
	if (pgb->current_page)
		pgb->current_page->next = page;
	page->prev = pgb->current_page;
	pgb->current_page = page;
	pgb->current_ptr = pgb__page_first_usable_slot(page);
	*aligned_size = pgb__page_align(alloc_size, page);
}

void *pgb_malloc(size_t size, pgb_t *pgb  MEMCALL_ARGS)
{
	pgb_byte *ptr;
	size_t aligned_size;
	if (size == 0)
		return NULL;

	if (!pgb->current_page) {
		pgb__add_page_for_alloc(pgb, size, &aligned_size);
	} else {
		aligned_size = pgb__page_align(size, pgb->current_page);
		if (pgb->current_ptr + aligned_size > pgb__page_end(pgb->current_page))
			pgb__add_page_for_alloc(pgb, size, &aligned_size);
	}
	ptr = pgb->current_ptr;
	pgb__alloc_set_sz(ptr, pgb->current_page, aligned_size);
	pgb->current_ptr += aligned_size;
	log_alloc("pgb", aligned_size  MEMCALL_VARS);
	return ptr;
}

void *pgb_calloc(size_t nmemb, size_t size, pgb_t *pgb  MEMCALL_ARGS)
{
	void *ptr = pgb_malloc(nmemb * size, pgb  MEMCALL_VARS);
	memset(ptr, 0, nmemb * size);
	return ptr;
}

static
void pgb__restore_current_page_ptr(pgb_t *pgb)
{
	pgb_page_t *page = pgb->current_page;
	for (size_t i = PGB__PAGE_SLOTS; i > 0; --i) {
		if (page->alloc_sizes[i-1] != 0) {
			pgb->current_ptr = &page->start +   (page->alloc_sizes[i-1] + i - 1)
			                                  * pgb__page_alignment(page);
			return;
		}
	}
	pgb->current_ptr = pgb__page_first_usable_slot(page);
}

void *pgb_realloc(void *ptr_, size_t size, pgb_t *pgb  MEMCALL_ARGS)
{
	pgb_byte *ptr = ptr_;
	if (ptr) {
		if (size) {
			if (   pgb->current_page
			    && pgb__ptr_is_last_alloc(ptr, pgb)
			    &&    ptr + pgb__page_align(size, pgb->current_page)
			       <= pgb__page_end(pgb->current_page)) {
				const size_t aligned_size = pgb__page_align(size, pgb->current_page);
				pgb__alloc_set_sz(ptr, pgb->current_page, aligned_size);
				pgb->current_ptr = ptr + aligned_size;
				assert(pgb->current_ptr <= pgb__page_end(pgb->current_page));
				return ptr;
			} else {
				const pgb_page_t *page = pgb->current_page;
				while (page && !pgb__ptr_in_page(ptr, page))
					page = page->prev;
				error_if(!page, "could not find page for allocation");
				if (pgb__alloc_get_sz(ptr, page) >= size) {
					return ptr;
				} else {
					pgb_byte *new_ptr = pgb_malloc(size, pgb  MEMCALL_VARS);
					memcpy(new_ptr, ptr, pgb__alloc_get_sz(ptr, page));
					return new_ptr;
				}
			}
		} else {
			pgb_free(ptr, pgb  MEMCALL_VARS);
			return NULL;
		}
	} else if (size) {
		return pgb_malloc(size, pgb  MEMCALL_VARS);
	} else {
		return NULL;
	}
}

void pgb_free(void *ptr, pgb_t *pgb  MEMCALL_ARGS)
{
	if (pgb__ptr_is_last_alloc(ptr, pgb)) {
		pgb__alloc_set_sz(ptr, pgb->current_page, 0);
		pgb->current_ptr = ptr;
		if (ptr == pgb__page_first_usable_slot(pgb->current_page)) {
			pgb_page_t *prev = pgb->current_page->prev;
			pgb_heap_return_page(pgb->heap, pgb->current_page);
			pgb->current_page = prev;
			if (pgb->current_page) {
				pgb__restore_current_page_ptr(pgb);
				pgb->current_page->next = NULL;
			}
		}
	} else if (pgb__ptr_in_page(ptr, pgb->current_page)) {
		pgb__alloc_set_sz(ptr, pgb->current_page, 0);
#ifdef VLT_ANALYZE_TEMP_MEMORY
		log_warn("pgb_free: cannot recapture memory @ %s", loc);
#endif
	}
#ifdef VLT_ANALYZE_TEMP_MEMORY
	else
		log_warn("pgb_free: cannot recapture memory @ %s", loc);
#endif
}

pgb_watermark_t pgb_save(pgb_t *pgb)
{
	return (pgb_watermark_t) {
		.pgb  = pgb,
		.page = pgb->current_page,
		.ptr  = pgb->current_ptr,
	};
}

void pgb_restore(pgb_watermark_t watermark)
{
	pgb_t *pgb = watermark.pgb;
	pgb_page_t *page = pgb->current_page;
	while (page != watermark.page) {
		pgb_page_t *prev = page->prev;
		pgb_heap_return_page(pgb->heap, page);
		page = prev;
	}
	pgb->current_page = watermark.page;
	pgb->current_ptr  = watermark.ptr;
	if (pgb->current_page) {
		pgb_byte *sz_slot = pgb__alloc_get_sz_slot(pgb->current_ptr, pgb->current_page);
		memset(sz_slot, 0, &pgb->current_page->header_begin - sz_slot);
		pgb->current_page->next = NULL;
	}
}

b32 pgb_owns_ptr(const pgb_t *pgb, const void *ptr)
{
	const pgb_page_t *page = pgb->current_page;
	while (page && !pgb__ptr_in_page(ptr, page))
		page = page->prev;
	return page != NULL;
}

void pgb_stats(const pgb_t *pgb, size_t *bytes_used, size_t *pages_used,
               size_t *bytes_available, size_t *pages_available)
{
	pgb_page_t *page;

	*bytes_used = 0;
	*pages_used = 0;
	page = pgb->current_page;
	if (page) {
		*bytes_used += pgb->current_ptr - pgb__page_first_usable_slot(page);
		++*pages_used;
		page = page->prev;
	}
	while (page) {
		*bytes_used += page->size;
		++*pages_used;
		page = page->prev;
	}

	*bytes_available = 0;
	*pages_available = 0;
	page = pgb->heap->first_page;
	while (page) {
		*bytes_available += page->size;
		++*pages_available;
		page = page->next;
	}
}

void pgb_log(const pgb_t *pgb, const char *name)
{
	pgb_page_t *current_page = pgb->current_page;
	pgb_page_t *page = current_page;

	if (name)
		log_debug("%s pgb", name);

	if (!page) {
		log_debug("empty");
		return;
	}

	while (page->prev)
		page = page->prev;
	while (page != current_page) {
		log_debug("%p: %lu", page, page->size);
		page = page->next;
	}

	log_debug("%p: %lu/%lu", current_page, pgb->current_ptr - (pgb_byte*)current_page, current_page->size);
}

void pgb_log_heap(const pgb_t *pgb)
{
	pgb_heap_t *heap = pgb->heap;
	pgb_page_t *page = heap->first_page;
	log_debug("pgb heap");
	while (page) {
		log_debug("%p: %lu", page, page->size);
		page = page->next;
	}
}

#undef PGB_IMPLEMENTATION
#endif // PGB_IMPLEMENTATION
