#ifndef PGB_H
#define PGB_H

/* PaGed Bump allocator */

typedef struct pgb
{
	struct pgb_page *current_page;
	u8 *current_ptr;
} pgb_t;

void pgb_init(pgb_t *pgb);
void pgb_destroy(pgb_t *pgb);

void *pgb_malloc(size_t size, allocator_t *a  MEMCALL_ARGS);
void *pgb_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS);
void *pgb_realloc(void *ptr, size_t size, allocator_t *a  MEMCALL_ARGS);
void  pgb_free(void *ptr, allocator_t *a  MEMCALL_ARGS);

typedef struct pg_watermark
{
	struct pgb_page *page;
	u8 *ptr;
} pgb_watermark_t;

pgb_watermark_t pgb_save(const pgb_t *pgb);
void            pgb_restore(pgb_t *pgb, pgb_watermark_t watermark);

void pgb_stats(const pgb_t *pgb, u32 *byte_cnt, u32 *page_cnt);

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

#define PGB_MIN_PAGE_SIZE        4096
#define PGB_PAGE_SLOTS           256
#define pgb_alignment(page_size) ((page_size) / PGB_PAGE_SLOTS)
#define pgb_size_t               u32

typedef struct pgb_page
{
	union
	{
		u8 start;
		u8 alloc_sizes[PGB_PAGE_SLOTS];
	};
	union
	{
		u8 header_begin;
		struct
		{
			struct pgb_page *prev, *next;
			pgb_size_t size;
			u8 header_end;
		};
		u8 first_slot;
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

static_assert(sizeof(pgb__max_align_t) <= pgb_alignment(PGB_MIN_PAGE_SIZE),
              invalid_word_size_for_pgb_allocator);

#define pgb_page_alignment(page)   (pgb_alignment((page)->size))
#define pgb_page_end(page)         (&(page)->start + (page)->size)
#define pgb__header_size()         (  offsetof(pgb_page_t, header_end) \
                                    - offsetof(pgb_page_t, header_begin))
#define pgb_header_size(page_size) (pgb_align(pgb__header_size(), \
                                              pgb_alignment(page_size)))

static inline
pgb_size_t pgb_align(pgb_size_t size, pgb_size_t alignment)
{
	return size + (alignment - (size % alignment)) % alignment;
}

static inline
pgb_size_t pgb_page_align(pgb_size_t size, const pgb_page_t *page)
{
	return pgb_align(size, pgb_page_alignment(page));
}

static inline
pgb_size_t pgb__alloc_get_sz(const u8* ptr, const pgb_page_t *page)
{
	const pgb_size_t alignment = pgb_page_alignment(page);
	return page->alloc_sizes[(ptr - &page->start) / alignment] * alignment;
}

static inline
void pgb__alloc_set_sz(const u8 *ptr, pgb_page_t *page, pgb_size_t sz)
{
	const pgb_size_t alignment = pgb_page_alignment(page);
	page->alloc_sizes[(ptr - &page->start) / alignment] = (u8)(sz / alignment);
}

static inline
b32 pgb__ptr_in_page(const u8 *ptr, const pgb_page_t *page)
{
	return ptr > &page->start && ptr < pgb_page_end(page);
}

static
b32 pgb__ptr_is_last_alloc(const u8 *ptr, const pgb_t *pgb)
{
	return   pgb__ptr_in_page(ptr, pgb->current_page)
	      && ptr + pgb__alloc_get_sz(ptr, pgb->current_page) == pgb->current_ptr;
}

static
void pgb__remove_page(pgb_page_t *page)
{
	page->prev->next = page->next;
	if (page->next)
		page->next->prev = page->prev;
}

static
void pgb__insert_page(pgb_page_t *page, pgb_page_t *prev, pgb_page_t *next)
{
	assert(prev != next || !prev);
	assert(page != prev);
	assert(page != next);
	assert(!prev || prev->prev != page);
	assert(!next || next->next != page);
	page->prev = prev;
	page->next = next;
	if (prev)
		prev->next = page;
	if (next)
		next->prev = page;
}

static
void pgb__create_page(pgb_t *pgb, pgb_size_t page_size,
                      pgb_page_t *prev, pgb_page_t *next)
{
	const pgb_size_t header_size = pgb_header_size(page_size);

	pgb->current_page = std_malloc(page_size);
	memset(pgb->current_page, 0, PGB_PAGE_SLOTS);
	pgb->current_page->size = page_size;
	pgb__alloc_set_sz(&pgb->current_page->header_begin,
	                  pgb->current_page, header_size);
	pgb->current_ptr = &pgb->current_page->first_slot + header_size;
	pgb__insert_page(pgb->current_page, prev, next);
}

void pgb_init(pgb_t *pgb)
{
	pgb__create_page(pgb, PGB_MIN_PAGE_SIZE, NULL, NULL);
}

void pgb_destroy(pgb_t *pgb)
{
	pgb_page_t *tail = pgb->current_page;
	while (tail->next)
		tail = tail->next;
	do {
		pgb_page_t *prev = tail->prev;
		std_free(tail);
		tail = prev;
	} while (tail);
}

// #include <intrin.h>

static
void pgb__find_page_for_alloc(pgb_t *pgb, pgb_size_t alloc_size)
{
	const pgb_size_t aligned_size = 1 << (32 - __builtin_clz(alloc_size));
	pgb_size_t page_size =   PGB_MIN_PAGE_SIZE > aligned_size
	                       ? PGB_MIN_PAGE_SIZE : aligned_size;
	if (alloc_size > page_size - pgb_header_size(page_size))
		page_size <<= 1;

	if (page_size == PGB_MIN_PAGE_SIZE) {
		/* search forward */
		pgb_page_t *next = pgb->current_page->next;
		if (next && next->size == PGB_MIN_PAGE_SIZE) {
			pgb->current_page = next;
			pgb->current_ptr =   &pgb->current_page->first_slot
			                   + pgb_header_size(PGB_MIN_PAGE_SIZE);
		} else {
			pgb__create_page(pgb, page_size, pgb->current_page, next);
		}
	} else {
		/* search backward */
		pgb_page_t *page = pgb->current_page;
		while (page->next)
			page = page->next;
		while (page->size > page_size && page->prev != pgb->current_page)
			page = page->prev;
		if (page->size == page_size && page != pgb->current_page) {
			/*if (page != pgb->current_page->next)
				page->prev->next = page->next;
			if (page->next)
				page->next->prev = page->prev;*/

			if (page != pgb->current_page->next) {
				pgb__remove_page(page);
				pgb__insert_page(page, pgb->current_page, pgb->current_page->next);
			}

			/*page->prev = pgb->current_page;
			page->next = pgb->current_page->next;

			if (pgb->current_page->next && page != pgb->current_page->next) {
				pgb->current_page->next->prev = page;
				pgb->current_page->next = page;
			}*/

			pgb->current_page = page;
			pgb->current_ptr =   &pgb->current_page->first_slot
			                   + pgb_header_size(page_size);
		} else {
			pgb__create_page(pgb, page_size, pgb->current_page, pgb->current_page->next);
		}
	}
}

void *pgb_malloc(size_t size, allocator_t *a  MEMCALL_ARGS)
{
	pgb_t *pgb = a->udata;
	u8 *ptr;
	pgb_size_t aligned_size;
	if (size == 0)
		return NULL;

	aligned_size = pgb_page_align(size, pgb->current_page);
	if (pgb->current_ptr + aligned_size > pgb_page_end(pgb->current_page)) {
		pgb__find_page_for_alloc(pgb, size);
		aligned_size = pgb_page_align(size, pgb->current_page);
	}
	ptr = pgb->current_ptr;
	pgb__alloc_set_sz(ptr, pgb->current_page, aligned_size);
	pgb->current_ptr += aligned_size;
	log_alloc("pgb", aligned_size  MEMCALL_VARS);
	return ptr;
}

void *pgb_calloc(size_t nmemb, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	void *ptr = pgb_malloc(nmemb * size, a  MEMCALL_VARS);
	memset(ptr, 0, nmemb * size);
	return ptr;
}

static
void pgb__close_current_page(pgb_t *pgb)
{
	pgb_page_t *page = pgb->current_page;
	pgb_page_t *prev = pgb->current_page->prev;
	pgb_page_t *insert_after = page;

	while (insert_after->next && page->size > insert_after->next->size)
		insert_after = insert_after->next;
	if (insert_after != page) {
		pgb__remove_page(page);
		pgb__insert_page(page, insert_after, insert_after->next);
	}

	if (prev) {
		pgb->current_page = prev;
	} else {
		pgb->current_page = insert_after;
		while (pgb->current_page->prev)
			pgb->current_page = pgb->current_page->prev;
	}
}

static
void pgb__restore_current_page_ptr(pgb_t *pgb)
{
	pgb_page_t *page = pgb->current_page;
	for (u32 i = PGB_PAGE_SLOTS; i > 0; --i) {
		if (page->alloc_sizes[i-1] != 0) {
			pgb->current_ptr = &page->start +   (page->alloc_sizes[i-1] + i - 1)
			                                  * pgb_page_alignment(page);
			return;
		}
	}
	pgb->current_ptr = &page->first_slot + pgb_header_size(page->size);
}

static
void pgb__free(void *ptr, allocator_t *a, b32 close  MEMCALL_ARGS)
{
	pgb_t *pgb = a->udata;
	if (pgb__ptr_is_last_alloc(ptr, pgb)) {
		pgb__alloc_set_sz(ptr, pgb->current_page, 0);
		pgb->current_ptr = ptr;
		if (   ptr ==   &pgb->current_page->first_slot
		              + pgb_header_size(pgb->current_page->size)
		    && pgb->current_page->prev
		    && close) {
			pgb__close_current_page(pgb);
			pgb__restore_current_page_ptr(pgb);
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

void *pgb_realloc(void *ptr_, size_t size, allocator_t *a  MEMCALL_ARGS)
{
	u8 *ptr = ptr_;
	pgb_t *pgb = a->udata;
	if (ptr) {
		if (size) {
			if (   pgb__ptr_is_last_alloc(ptr, pgb)
			    && ptr + size <= pgb_page_end(pgb->current_page)) {
				pgb__free(ptr, a, false  MEMCALL_VARS);
				return pgb_malloc(size, a  MEMCALL_VARS);
			} else {
				const pgb_page_t *page = pgb->current_page;
				u8 *new_ptr;
				while (page && !pgb__ptr_in_page(ptr, page))
					page = page->prev;
				error_if(!page, "could not find page for allocation");
				new_ptr = pgb_malloc(size, a  MEMCALL_VARS);
				memcpy(new_ptr, ptr, pgb__alloc_get_sz(ptr, page));
				return new_ptr;
			}
		} else {
			pgb_free(ptr, a  MEMCALL_VARS);
			return NULL;
		}
	} else if (size) {
		return pgb_malloc(size, a  MEMCALL_VARS);
	} else {
		return NULL;
	}
}

void pgb_free(void *ptr, allocator_t *a  MEMCALL_ARGS)
{
	pgb__free(ptr, a, true  MEMCALL_VARS);
}

pgb_watermark_t pgb_save(const pgb_t *pgb)
{
	return (pgb_watermark_t) {
		.page = pgb->current_page,
		.ptr  = pgb->current_ptr,
	};
}

void pgb_restore(pgb_t *pgb, pgb_watermark_t watermark)
{
	while (pgb->current_page != watermark.page)
		pgb__close_current_page(pgb);
	pgb->current_page   = watermark.page;
	pgb->current_ptr    = watermark.ptr;
}

void pgb_stats(const pgb_t *pgb, u32 *byte_cnt_, u32 *page_cnt_)
{
	u32 byte_cnt = 0, page_cnt = 0;
	pgb_page_t *head = pgb->current_page;
	while (head->prev)
		head = head->prev;
	do {
		pgb_page_t *next = head->next;
		byte_cnt += head->size;
		++page_cnt;
		head = next;
	} while (head);
	*byte_cnt_ = byte_cnt;
	*page_cnt_ = page_cnt;
}

#undef PGB_IMPLEMENTATION
#endif // PGB_IMPLEMENTATION
