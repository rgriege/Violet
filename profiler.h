#ifndef VIOLET_PROFILER_H
#define VIOLET_PROFILER_H

#define PROFILE_BLOCK_COUNT 32

void profile_block_begin(const char *name);
void profile_block_end(const char *name);
void profile_aggregate(void);
void profile_reset(void);
void profile_clear_all(void);

#ifdef PROFILE
#define PROFILE_BLOCK_BEGIN(name)           profile_block_begin(#name)
#define PROFILE_BLOCK_END(name)             profile_block_end(#name)
#define PROFILE_FUNCTION_BEGIN()            profile_block_begin(__FUNCTION__)
#define PROFILE_FUNCTION_END()              profile_block_end(__FUNCTION__)
#define PROFILE_AGGREGATE()                 profile_aggregate()
#define PROFILE_RESET()                     profile_reset()
#define PROFILE_CLEAR_ALL()                 profile_clear_all()
#else
#define PROFILE_BLOCK_BEGIN(name)           NOOP
#define PROFILE_BLOCK_END(name)             NOOP
#define PROFILE_FUNCTION_BEGIN()            NOOP
#define PROFILE_FUNCTION_END()              NOOP
#define PROFILE_AGGREGATE()                 NOOP
#define PROFILE_RESET()                     NOOP
#define PROFILE_CLEAR_ALL()                 NOOP
#endif

#endif // VIOLET_PROFILER_H

#ifdef PROFILER_IMPLEMENTATION

typedef struct profile__block
{
	const char *name; /* non-owned, only NULL for unused blocks */
	u32 id;
	u32 microseconds;
	u32 count;
	u32 depth;
	b32 aggregate;
	timepoint_t last_start;
	struct profile__block *parent; /* non-owned, only NULL for root */
	struct profile__block *child; /* non-owned, can be NULL */
	struct profile__block *sibling; /* non-owned, can be NULL */
} profile__block_t;

thread_local profile__block_t g_profiler_blocks[PROFILE_BLOCK_COUNT] = {0};
thread_local profile__block_t *g_profiler_block_last = NULL; /* NULL at frame start */

static
b32 profile__block_find(u32 id, profile__block_t **block)
{
	for (u32 i = 0; i < countof(g_profiler_blocks); ++i) {
		if (   g_profiler_blocks[i].id == id
		    && g_profiler_blocks[i].parent == g_profiler_block_last) {
			*block = &g_profiler_blocks[i];
			return true;
		}
	}
	return false;
}

static
b32 profile__block_find_empty(profile__block_t **block)
{
	for (u32 i = 0; i < countof(g_profiler_blocks); ++i) {
		if (g_profiler_blocks[i].name == NULL) {
			*block = &g_profiler_blocks[i];
			return true;
		}
	}
	return false;
}

static
b32 profiler__block_in_siblings(const profile__block_t *siblings,
                                const profile__block_t *block)
{
	while (siblings && block != siblings)
		siblings = siblings->sibling;
	return siblings != NULL;
}

void profile_block_begin(const char *name)
{
	const u32 id = hash_compute(name);
	profile__block_t *block = NULL;

	if (   !profile__block_find(id, &block)
	    && !profile__block_find_empty(&block)) {
		assert(false);
		return;
	}

	block->name       = name;
	block->id         = id;
	block->depth      = 0;
	block->aggregate  = false;
	block->last_start = timepoint_create();
	block->parent     = g_profiler_block_last;
	if (g_profiler_block_last) {
		block->depth      = g_profiler_block_last->depth + 1;
		block->aggregate  = g_profiler_block_last->aggregate;
		if (!profiler__block_in_siblings(g_profiler_block_last->child, block)) {
			block->sibling  = g_profiler_block_last->child;
			g_profiler_block_last->child = block;
		}
	}
	g_profiler_block_last = block;
}

static
void profile__block_reverse_children(profile__block_t *block)
{
	profile__block_t *first = block->child;
	profile__block_t *prev  = block->child;
	profile__block_t *child = block->child->sibling;
	while (child) {
		profile__block_t *sibling = child->sibling;
		child->sibling = prev;
		prev  = child;
		child = sibling;
	}
	first->sibling = NULL;
	block->child = prev;

	child = block->child;
	do {
		if (child->child)
			profile__block_reverse_children(child);
		child = child->sibling;
	} while (child);
}

static
void profile__block_log(const profile__block_t *block)
{
	profile__block_t *child = block->child;
	while (child) {
		profile__block_log(child);
		child = child->sibling;
	}

#ifdef DEBUG
	/* try to make it extra clear that these are debug timings */
	log_debug("PROFILE: %*s%s = %s us (%u) [DEBUG]", block->depth, "", block->name,
	          imprint_u32(block->microseconds), block->count);
#else
	log_info("PROFILE: %*s%s = %s us (%u)", block->depth, "", block->name,
	         imprint_u32(block->microseconds), block->count);
#endif
}

static
void profile__block_clear(profile__block_t *block)
{
	profile__block_t *child = block->child;
	while (child) {
		profile__block_t *sibling = child->sibling;
		profile__block_clear(child);
		child = sibling;
	}

	memclr(*block);
}

void profile_block_end(const char *name)
{
	const timepoint_t end = timepoint_create();
	const u32 id = hash_compute(name);
	profile__block_t *block = g_profiler_block_last;

	assert(block);

	if (block->id != id) {
		assert(false);
		return;
	}

	block->count        += 1;
	block->microseconds += timepoint_diff_micro(block->last_start, end);

	g_profiler_block_last = block->parent;

	if (!block->aggregate) {
		/* arrange the children so that the first child added is logged first */
		if (block->child)
			profile__block_reverse_children(block);

		profile__block_log(block);

		if (g_profiler_block_last)
			g_profiler_block_last->child = block->sibling;

		profile__block_clear(block);
	}
}

void profile_aggregate(void)
{
	/* must be inside a block to aggregate - AGGREGATE should immediately follow BEGIN */
	assert(g_profiler_block_last);
	/* aggregated block must have a parent, otherwise we won't know when to spit out
	 * the aggregated data.  The urrent block's BEGIN must be prececed by another BEGIN
	 * somewhere up the call stack. */
	assert(g_profiler_block_last->parent);
	g_profiler_block_last->aggregate = true;
}

void profile_reset(void)
{
	assert(!g_profiler_block_last);
}

void profile_clear_all(void)
{
	arrclr(g_profiler_blocks);
	g_profiler_block_last = NULL;
}

#undef PROFILER_IMPLEMENTATION
#endif // PROFILER_IMPLEMENTATION
