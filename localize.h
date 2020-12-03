#ifndef VIOLET_LOCALIZE_H
#define VIOLET_LOCALIZE_H

#ifndef NO_LOCALIZE

#ifndef LOCALIZE_MAX_STR_SLOTS
#define LOCALIZE_MAX_STR_SLOTS 20
#endif

#ifndef LOCALIZE_MAX_STR_BYTES
#define LOCALIZE_MAX_STR_BYTES 256
#endif

#define LOCALIZE(str) localize_string(str)
#define LOCALIZE_STATIC(str) str

const char *localize_string(const char *str);

b32  localize_is_language_default(void);
void localize_set_language_default(const char *fname);
b32  localize_set_language(const char *fname);
b32  localize_save_language(const char *fname);

typedef struct localized_string {
	u32 id;
	u32 index;
} localized_string_t;

typedef struct localization_table {
	localized_string_t slots[LOCALIZE_MAX_STR_SLOTS];
	char chars[LOCALIZE_MAX_STR_BYTES];
} localization_table_t;

b32  localization_table_load(localization_table_t *table, const char *fname);
b32  localization_table_save(const localization_table_t *table, const char *fname);
void localization_table_sort(localization_table_t *table);
b32  localization_table_find_slot(localization_table_t *table, u32 id,
                                  localized_string_t **slot);
u32  localization_table_num_strings(const localization_table_t *table);
u32  localization_table_num_bytes(const localization_table_t *table);

#else // NO_LOCALIZE

#define LOCALIZE(str) str
#define LOCALIZE_STATIC(str) str
b32  localize_is_language_default(void);
void localize_set_language_default(const char *fname);
b32  localize_set_language(const char *fname);
b32  localize_save_language(const char *fname);

#endif // NO_LOCALIZE

#endif // VIOLET_LOCALIZE_H

#ifdef LOCALIZE_IMPLEMENTATION

#ifndef NO_LOCALIZE
u32 g_lang = 0;
u32 g_lang_default = 0;

localization_table_t g_localization_table = {0};

const char *localize_string(const char *str)
{
	if (g_lang == g_lang_default)
		return str;

	localized_string_t *slot;
	if (   localization_table_find_slot(&g_localization_table, hash_compute(str), &slot)
	    && slot->id != 0) {
		return &g_localization_table.chars[slot->index];
	} else {
#ifdef DEBUG
		static b32 s_warned_missing_localized_string = false;
		if (slot->id == 0 && !s_warned_missing_localized_string) {
			log_warn("missing localization for string %s", str);
			s_warned_missing_localized_string = true;
		}
#endif
		return str;
	}
}

b32 localize_is_language_default(void)
{
	return g_lang == g_lang_default;
}

void localize_set_language_default(const char *fname)
{
	g_lang_default = hash_compute(fname);
}

b32 localize_set_language(const char *fname)
{
	const u32 lang = hash_compute(fname);

	if (g_lang == lang)
		return true;

	if (lang == g_lang_default) {
		g_lang = g_lang_default;
		return true;
	}

	u32 new_lang = g_lang_default;

	if (!localization_table_load(&g_localization_table, fname)) {
		memclr(g_localization_table);
		goto out;
	}

	log_info("switched to language %s", fname);
	log_debug("localization table will use %u/%u slots",
	          localization_table_num_strings(&g_localization_table),
	          LOCALIZE_MAX_STR_SLOTS);
	log_debug("localization table will use %u/%u bytes",
	          localization_table_num_bytes(&g_localization_table),
	          LOCALIZE_MAX_STR_BYTES);

	new_lang = lang;

out:
	g_lang = new_lang;
	return g_lang == lang;
}

b32 localize_save_language(const char *fname)
{
	return localization_table_save(&g_localization_table, fname);
}

b32 localization_table_load(localization_table_t *table, const char *fname)
{
	b32 success = false;
	FILE *fp = file_open(fname, "rb");
	if (!fp) {
		log_error("failed to open language file '%s'", fname);
		return false;
	}

	memclr(*table);

	u32 num_strings = 0;
	if (!vson_read_u32(fp, "num_strings", &num_strings)) {
		log_error("failed to read language.num_strings");
		goto out;
	}
	if (num_strings > countof(table->slots)) {
		log_error("too many language strings");
		goto out;
	}

	u32 next_index = 1; // skip byte 0 so 'empty' slots point to an empty string

	for (u32 i = 0; i < num_strings; ++i) {
		u32 id;
		if (!vson_read_u32(fp, "id", &id)) {
			log_error("failed to read language.id #%u", i);
			goto out;
		}

		localized_string_t *slot = NULL;
		check(localization_table_find_slot(table, id, &slot));
		if (slot->id != 0)
			log_warn("duplicate language string entry '%u'", id);

		if (!vson_read_str(fp, "str", &table->chars[next_index],
		                   countof(table->chars)-1-next_index)) {
			log_error("failed to read language.str #%u", i);
			goto out;
		}

		const u32 len = (u32)strlen(&table->chars[next_index]);
		slot->id = id;
		slot->index = next_index;
		table->chars[slot->index+len] = 0;
		next_index += len + 1;
	}
	success = true;

out:
	fclose(fp);
	return success;
}

b32 localization_table_save(const localization_table_t *table, const char *fname)
{
	FILE *fp = file_open(fname, "wb");
	if (!fp)
		return false;

	u32 num_strings = 0;
	for (u32 i = 0; i < countof(table->slots); ++i)
		if (table->slots[i].id != 0)
			++num_strings;

	vson_write_u32(fp, "num_strings", num_strings);
	for (u32 i = 0; i < countof(table->slots); ++i) {
		if (table->slots[i].id != 0) {
			const localized_string_t *string = &table->slots[i];
			vson_write_u32(fp, "id", string->id);
			vson_write_str(fp, "str", &table->chars[string->index]);
		}
	}

	fclose(fp);
	return true;
}

static
int localization__slot_sort_asc(const void *lhs_, const void *rhs_)
{
	const localized_string_t *lhs = lhs_, *rhs = rhs_;
	return lhs->id < rhs->id ? -1 : lhs->id > rhs->id ? 1 : 0;
}

void localization_table_sort(localization_table_t *table)
{
	qsort(table->slots, LOCALIZE_MAX_STR_SLOTS, sizeof(table->slots[0]),
	      localization__slot_sort_asc);
}

b32 localization_table_find_slot(localization_table_t *table, u32 id,
                                 localized_string_t **slot)
{
	const u32 n = countof(table->slots);
	u32 index = id % n;
	if (table->slots[index].id == id || table->slots[index].id == 0) {
		*slot = &table->slots[index];
		return true;
	}
	for (const u32 orig = index++; index != orig; index = (index + 1) % n) {
		if (table->slots[index].id == id || table->slots[index].id == 0) {
			*slot = &table->slots[index];
			return true;
		}
	}
	return false;
}

u32 localization_table_num_strings(const localization_table_t *table)
{
	u32 num_strings = 0;
	for (u32 i = 0; i < countof(table->slots); ++i)
		if (table->slots[i].id != 0)
			++num_strings;
	return num_strings;
}

u32 localization_table_num_bytes(const localization_table_t *table)
{
	u32 num_bytes;
	for (num_bytes = countof(table->chars); num_bytes > 0; --num_bytes)
		if (table->chars[num_bytes-1])
			break;
	return num_bytes;
}

#else // NO_LOCALIZE

u32 g_lang = 0;
u32 g_lang_default = 0;

b32 localize_is_language_default(void)
{
	return g_lang == g_lang_default;
}

void localize_set_language_default(const char *fname)
{
	g_lang_default = hash_compute(fname);
}

b32  localize_set_language(const char *fname)
{
	if (hash_compute(fname) != g_lang_default)
		return false;

	g_lang = g_lang_default;
	return true;
}

b32  localize_save_language(const char *fname)
{
	return false;
}

#endif // NO_LOCALIZE

#undef LOCALIZE_IMPLEMENTATION
#endif // LOCALIZE_IMPLEMENTATION
