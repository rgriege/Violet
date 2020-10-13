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

#ifndef PGB_MIN_PAGE_SIZE
#define PGB_MIN_PAGE_SIZE 4096
#endif

#ifndef PGB_MALLOC
#define PGB_MALLOC malloc
#endif

#ifndef PGB_FREE
#define PGB_FREE free
#endif

#ifdef PGB_TRACK_MEMORY
#define MEMCALL_LOCATION , LOCATION
#define MEMCALL_ARGS , const char *loc
#define MEMCALL_VARS , loc
#else
#define MEMCALL_LOCATION
#define MEMCALL_ARGS
#define MEMCALL_VARS
#endif

#ifndef PGB_LOG
#define PGB_LOG(...)
#endif

#ifndef PGB_LOG_ALLOC
#define PGB_LOG_ALLOC(...)
#endif

#ifndef PGB_LOG_REALLOC
#define PGB_LOG_REALLOC(...)
#endif

typedef struct pgb_heap
{
	struct pgb_page *first_page;
} pgb_heap_t;

void pgb_heap_init(pgb_heap_t *heap);
void pgb_heap_destroy(pgb_heap_t *heap);

struct pgb_page *pgb_heap_borrow_page(pgb_heap_t *heap, size_t min_size, size_t max_size);
void             pgb_heap_return_page(pgb_heap_t *heap, struct pgb_page *page);
void             pgb_heap_create_page(pgb_heap_t *heap, size_t size);

void pgb_heap_move_all_pages(pgb_heap_t *dst, pgb_heap_t *src);

typedef uint8_t pgb_byte;

typedef struct pgb_watermark_data
{
	struct pgb *pgb;
	struct pgb_page *page;
	pgb_byte *ptr;
} pgb_watermark_data_t;

typedef struct pgb_watermark
{
	pgb_watermark_data_t data;
#ifdef DEBUG
	pgb_watermark_data_t prev; /* stored in debug to validate reallocations */
#endif
} pgb_watermark_t;

typedef struct pgb
{
	struct pgb_heap *heap;
	struct pgb_page *current_page;
	pgb_byte *current_ptr;
	pgb_watermark_t last_mark;
} pgb_t;

void pgb_init(pgb_t *pgb, pgb_heap_t *heap);
void pgb_destroy(pgb_t *pgb);

void *pgb_malloc(size_t size, pgb_t *pgb  MEMCALL_ARGS);
void *pgb_calloc(size_t nmemb, size_t size, pgb_t *pgb  MEMCALL_ARGS);
void *pgb_realloc(void *ptr, size_t size, pgb_t *pgb  MEMCALL_ARGS);
void  pgb_free(void *ptr, pgb_t *pgb  MEMCALL_ARGS);

pgb_watermark_t pgb_save(pgb_t *pgb);
void            pgb_restore(pgb_watermark_t watermark);

size_t pgb_alloc_size(const pgb_t *pgb, const void *ptr);

void pgb_stats(const pgb_t *pgb, size_t *bytes_used, size_t *pages_used,
               size_t *bytes_available, size_t *pages_available);
void pgb_watermark_stats(pgb_watermark_t mark, size_t *bytes_used, size_t *pages_used);

#endif // PGB_H



/* Implementation */
#ifdef PGB_IMPLEMENTATION

/*
 * Divides pages into 256 equal-sized slots.
 * An allocated can occupy more than 1 slot, but is aligned to a whole number of slots.
 * The sizes are stored in a single byte representing the number of slots.
 * This causes the header size to be constant, which is more efficent for larger pages.
 *
 * Perhaps the biggest drawback is the loss of efficiency when allocating
 * small objects into large pages.
 *
 * Not all slots will be available, since some are required to store the page header.
 * Num slots = max(1, sizeof(header) / alignment)
 *     4K  ->   16 byte alignment -> 238 usable slots -> 93% efficient
 *     8K  ->   32 byte alignment -> 247 usable slots -> 96% efficient
 *    64K  ->  256 byte alignment -> 254 usable slots -> 99% efficient
 *   128K  ->  512 byte alignment -> 255 usable slots -> 99% efficient
 *
 * Unused pages are stored in a heap that can be shared across multiple allocators.
 * This heap is not thread-safe.
 */


/* Page */

#define PGB__PAGE_SLOTS            256
#define pgb__alignment(page_size)  ((page_size) / PGB__PAGE_SLOTS)

typedef struct pgb_page
{
	struct {
		pgb_byte alloc_sizes[PGB__PAGE_SLOTS];
		struct pgb_page *prev;
		struct pgb_page *next;
		size_t size;
		pgb_byte header_end;
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

#ifdef offsetof
#define pgb_offsetof(s, m) offsetof(s, m)
#else
#define pgb_offsetof(s, m) ((size_t)&((s*)(NULL))->m)
#endif

#ifndef pgb_assert
#include <assert.h>
#define pgb_assert(cnd) assert(cnd)
#endif

#define pgb_static_assert(cnd, msg) typedef int msg[(cnd) ? 1 : -1]

pgb_static_assert(sizeof(pgb__max_align_t) <= pgb__alignment(PGB_MIN_PAGE_SIZE),
                  invalid_word_size_for_pgb_allocator);

#define pgb__page_alignment(page)   (pgb__alignment((page)->size))
#define pgb__page_beg(page)         ((pgb_byte*)page)
#define pgb__page_end(page)         ((pgb_byte*)page + (page)->size)
#define pgb__header_size()          (pgb_offsetof(pgb_page_t, header_end))

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
	return pgb__page_beg(page) + pgb__align(pgb__header_size(), pgb__page_alignment(page));
}

static
size_t pgb__alloc_get_slot_idx(const pgb_byte* ptr, const pgb_page_t *page)
{
	return (ptr - pgb__page_beg(page)) / pgb__page_alignment(page);
}

static
pgb_byte *pgb__slot_get_alloc(pgb_page_t *page, size_t slot)
{
	return pgb__page_beg(page) + slot * pgb__page_alignment(page);
}

static
size_t pgb__alloc_get_sz(const pgb_byte* ptr, const pgb_page_t *page)
{
	const size_t alignment = pgb__page_alignment(page);
	return page->alloc_sizes[(ptr - pgb__page_beg(page)) / alignment] * alignment;
}

static
void pgb__alloc_set_sz(const pgb_byte *ptr, pgb_page_t *page, size_t sz)
{
	const size_t alignment = pgb__page_alignment(page);
	page->alloc_sizes[(ptr - pgb__page_beg(page)) / alignment] = (pgb_byte)(sz / alignment);
}

static
bool pgb__ptr_in_page(const pgb_byte *ptr, const pgb_page_t *page)
{
	return ptr > pgb__page_beg(page) && ptr < pgb__page_end(page);
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
	else if (  page_size
	         - pgb__align(pgb__header_size(), pgb__alignment(page_size))
	         < pgb__align(alloc_size, pgb__alignment(page_size)))
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
	return max_page_size > PGB_MIN_PAGE_SIZE ? max_page_size : PGB_MIN_PAGE_SIZE;
}

static
void pgb__page_clear(pgb_page_t *page)
{
	const size_t header_size = pgb__page_align(pgb__header_size(), page);
	memset(page->alloc_sizes, 0, PGB__PAGE_SLOTS);
	pgb__alloc_set_sz(pgb__page_beg(page), page, header_size);
	page->prev = NULL;
	page->next = NULL;
}

static
pgb_page_t *pgb__page_create(size_t page_size)
{
	pgb_page_t *page = PGB_MALLOC(page_size);
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
		PGB_FREE(page);
		page = next;
	}
	heap->first_page = NULL;
}

struct pgb_page *pgb_heap_borrow_page(pgb_heap_t *heap, size_t min_size, size_t max_size)
{
	pgb_assert(min_size <= max_size);
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

void pgb_heap_create_page(pgb_heap_t *heap, size_t size)
{
	pgb_heap_return_page(heap, pgb__page_create(size));
}

void pgb_heap_move_all_pages(pgb_heap_t *dst, pgb_heap_t *src)
{
	pgb_page_t *page = src->first_page;
	while (page) {
		pgb_page_t *next = page->next;
		pgb_heap_return_page(dst, page);
		page = next;
	}
	src->first_page = NULL;
}


/* Allocator */

void pgb_init(pgb_t *pgb, pgb_heap_t *heap)
{
	pgb->heap         = heap;
	pgb->current_page = NULL;
	pgb->current_ptr  = NULL;
	pgb->last_mark    = (pgb_watermark_t){0};
}

static
void pgb__pop_page(pgb_t *pgb)
{
	pgb_assert(pgb->current_page);
	pgb_page_t *prev = pgb->current_page->prev;
	pgb_heap_return_page(pgb->heap, pgb->current_page);
	pgb->current_page = prev;
	if (prev)
		prev->next = NULL;
}

#ifdef DEBUG
static
bool pgb__mark_valid(pgb_watermark_t mark)
{
	return mark.data.pgb && mark.data.page && mark.data.ptr;
}
#endif

void pgb_destroy(pgb_t *pgb)
{
	pgb_assert(!(pgb->current_page && pgb->current_page->next));
	pgb_assert(!pgb__mark_valid(pgb->last_mark));

	while (pgb->current_page)
		pgb__pop_page(pgb);
}

static
void pgb__add_page(pgb_t *pgb, pgb_page_t *page)
{
	if (pgb->current_page)
		pgb->current_page->next = page;
	page->prev = pgb->current_page;
	pgb->current_page = page;
	pgb->current_ptr = pgb__page_first_usable_slot(page);
}

static
void pgb__add_page_for_alloc(pgb_t *pgb, size_t alloc_size, size_t *aligned_size)
{
	const size_t min_page_size = pgb__page_min_size_for_alloc(alloc_size);
	const size_t max_page_size = pgb__page_max_size_for_alloc(alloc_size);
	pgb_page_t *page = pgb_heap_borrow_page(pgb->heap, min_page_size, max_page_size);
	pgb__add_page(pgb, page);
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
	PGB_LOG_ALLOC("pgb", aligned_size  MEMCALL_VARS);
	return ptr;
}

void *pgb_calloc(size_t nmemb, size_t size, pgb_t *pgb  MEMCALL_ARGS)
{
	void *ptr = pgb_malloc(nmemb * size, pgb  MEMCALL_VARS);
	memset(ptr, 0, nmemb * size);
	return ptr;
}

static
bool pgb__find_page_for_ptr(pgb_t *pgb, const void *ptr, pgb_page_t **ppage)
{
	pgb_page_t *page = pgb->current_page;
	while (page && !pgb__ptr_in_page(ptr, page))
		page = page->prev;
	if (page)
		*ppage = page;
	return page != NULL;
}

static
void pgb__restore_current_page_ptr(pgb_t *pgb, size_t slot)
{
	pgb_page_t *page = pgb->current_page;
	for (size_t i = slot; i > 0; --i) {
		if (page->alloc_sizes[i-1] != 0) {
			pgb_byte *ptr = pgb__slot_get_alloc(page, i-1);
			pgb->current_ptr = ptr + pgb__alloc_get_sz(ptr, page);
			return;
		}
	}
	pgb_assert(false); /* should have encountered header */
	pgb->current_ptr = pgb__page_first_usable_slot(page);
}

#ifdef DEBUG
static
bool pgb__ptr_freed_by_mark(pgb_watermark_t mark,
                            const pgb_page_t *page,
                            const pgb_byte *ptr)
{
	if (mark.data.page == page) {
		return ptr >= mark.data.ptr;
	} else {
		for (const pgb_page_t *p = mark.data.page->prev; p; p = p->prev)
			if (p == page)
				return false;
		return true;
	}
}
#endif

static
void *pgb__realloc(pgb_byte *ptr, size_t size, pgb_t *pgb  MEMCALL_ARGS)
{
	pgb_page_t *page;
	size_t old_size;

	if (!pgb__find_page_for_ptr(pgb, ptr, &page)) {
		pgb_assert(false);
#ifdef PGB_TRACK_MEMORY
		PGB_LOG("pgb_realloc: memory leak @ %s", loc);
#endif
		return NULL;
	}

	old_size = pgb__alloc_get_sz(ptr, page);

	/* Avoid the case where a pointer was not set to be freed by the last watermark,
	 * but it will be after reallocation. */
	pgb_assert(   !pgb__mark_valid(pgb->last_mark)
	           || pgb__ptr_freed_by_mark(pgb->last_mark, page, ptr));

	if ((old_size = pgb__alloc_get_sz(ptr, page)) >= size) {
		return ptr;
	} else if (   page == pgb->current_page
	           && ptr + old_size == pgb->current_ptr
	           &&    ptr + pgb__page_align(size, pgb->current_page)
	              <= pgb__page_end(pgb->current_page)) {
		const size_t aligned_size = pgb__page_align(size, pgb->current_page);
		pgb__alloc_set_sz(ptr, pgb->current_page, aligned_size);
		pgb->current_ptr = ptr + aligned_size;
		PGB_LOG_REALLOC("pgb", aligned_size  MEMCALL_VARS);
		return ptr;
	} else {
		pgb__alloc_set_sz(ptr, page, 0);
		pgb_byte *new_ptr = pgb_malloc(size, pgb  MEMCALL_VARS);
		memcpy(new_ptr, ptr, old_size);
		return new_ptr;
	}
}

void *pgb_realloc(void *ptr, size_t size, pgb_t *pgb  MEMCALL_ARGS)
{
	if (ptr) {
		if (size) {
			return pgb__realloc(ptr, size, pgb  MEMCALL_VARS);
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

void pgb_free(void *ptr_, pgb_t *pgb  MEMCALL_ARGS)
{
	pgb_byte *ptr = ptr_;
	pgb_page_t *page;
	size_t slot;
	size_t size;

	if (!ptr)
		return;

	if (!pgb__find_page_for_ptr(pgb, ptr, &page)) {
		pgb_assert(false);
#ifdef PGB_TRACK_MEMORY
		PGB_LOG("pgb_free: memory leak @ %s", loc);
#endif
		return;
	}

	slot = pgb__alloc_get_slot_idx(ptr, page);
	size = pgb__alloc_get_sz(ptr, page);
	pgb_assert(size != 0);
	pgb__alloc_set_sz(ptr, page, 0);

	if (ptr + size == pgb->current_ptr) {
		pgb__restore_current_page_ptr(pgb, slot);
		while (   pgb->current_page
		       && pgb->current_ptr == pgb__page_first_usable_slot(pgb->current_page)) {
			pgb__pop_page(pgb);
			if (pgb->current_page)
				pgb__restore_current_page_ptr(pgb, PGB__PAGE_SLOTS);
			else
				pgb->current_ptr = NULL;
		}
	} else if (page == pgb->current_page) {
#ifdef PGB_ANALYZE
		PGB_LOG("pgb_free: %u bytes out of order @ %s", size, loc);
#endif
	} else {
#ifdef PGB_ANALYZE
		PGB_LOG("pgb_free: %u bytes out of page @ %s", size, loc);
#endif
	}
}

pgb_watermark_t pgb_save(pgb_t *pgb)
{
	pgb_watermark_t mark = {
		.data = {
			.pgb  = pgb,
			.page = pgb->current_page,
			.ptr  = pgb->current_ptr,
		},
		.prev = pgb->last_mark.data,
	};
	pgb->last_mark.data = mark.data;
	return mark;
}

void pgb_restore(pgb_watermark_t watermark)
{
	pgb_t *pgb = watermark.data.pgb;
	while (pgb->current_page != watermark.data.page)
		pgb__pop_page(pgb);
	pgb->current_page = watermark.data.page;
	pgb->current_ptr  = watermark.data.ptr;
	if (pgb->current_page) {
		const size_t slot = pgb__alloc_get_slot_idx(pgb->current_ptr, pgb->current_page);
		memset(&pgb->current_page->alloc_sizes[slot], 0, PGB__PAGE_SLOTS - slot);
		pgb->current_page->next = NULL;
	}
	pgb->last_mark.data = watermark.prev;
}

size_t pgb_alloc_size(const pgb_t *pgb, const void *ptr)
{
	pgb_page_t *page;
	return pgb__find_page_for_ptr((pgb_t*)pgb, ptr, &page)
	     ? pgb__alloc_get_sz(ptr, page) : 0;
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

void pgb_watermark_stats(pgb_watermark_t mark, size_t *bytes_used, size_t *pages_used)
{
	pgb_t *pgb = mark.data.pgb;
	pgb_page_t *page = pgb->current_page;

	*bytes_used = 0;
	*pages_used = 0;

	if (page && page != mark.data.page) {
		*bytes_used += pgb->current_ptr - pgb__page_first_usable_slot(page);
		*pages_used += 1;
		page = page->prev;
	}

	while (page && page != mark.data.page) {
		*bytes_used += page->size;
		*pages_used += 1;
		page = page->prev;
	}

	if (page != mark.data.page) {
		*bytes_used = 0;
		*pages_used = 0;
		pgb_assert(false);
	} else if (page) {
		*bytes_used += pgb__page_end(page) - mark.data.ptr;
		*pages_used += 1;
	}
}

#undef PGB_IMPLEMENTATION
#endif // PGB_IMPLEMENTATION
