#ifndef VIOLET_LOCALIZE_H
#define VIOLET_LOCALIZE_H

#ifndef NO_LOCALIZE
#define LOCALIZE(str) localize_string(str)
#define LOCALIZE_STATIC(str) str

const char *localize_string(const char *str);

b32  localize_is_language_default(void);
void localize_set_language_default(const char *fname);
b32  localize_set_language(const char *fname);
b32  localize_save_language(const char *fname);

#ifndef LOCALIZE_MAX_STR_SLOTS
#define LOCALIZE_MAX_STR_SLOTS 20
#endif

#ifndef LOCALIZE_MAX_STR_BYTES
#define LOCALIZE_MAX_STR_BYTES 256
#endif

#else
#define LOCALIZE(str) str
#define LOCALIZE_STATIC(str) str
#endif

#endif // VIOLET_LOCALIZE_H

#ifdef LOCALIZE_IMPLEMENTATION

#ifndef NO_LOCALIZE
u32 g_lang = 0;
u32 g_lang_default = 0;

typedef struct localized_string {
	u32 id;
	u32 index;
} localized_string_t;

localized_string_t g_localized_strings[LOCALIZE_MAX_STR_SLOTS] = {0};

char g_localized_string_memory[LOCALIZE_MAX_STR_BYTES] = {0};

static
localized_string_t *localize__find_slot(u32 id)
{
	u32 index = id % LOCALIZE_MAX_STR_SLOTS;
	while (   g_localized_strings[index].id != id
	       && g_localized_strings[index].id != 0)
		index = (index + 1) % LOCALIZE_MAX_STR_SLOTS;
	return &g_localized_strings[index];
}

const char *localize_string(const char *str)
{
	if (g_lang == g_lang_default)
		return str;

	const localized_string_t *string = localize__find_slot(hash(str));
#ifdef DEBUG
	static b32 s_warned_missing_localized_string = false;
	if (string->id == 0 && !s_warned_missing_localized_string) {
		log_warn("missing localization for string %s");
		s_warned_missing_localized_string = true;
	}
#endif
	return string->id == 0 ? str : &g_localized_string_memory[string->index];
}

b32 localize_is_language_default(void)
{
	return g_lang == g_lang_default;
}

void localize_set_language_default(const char *fname)
{
	g_lang_default = hash(fname);
}

b32 localize_set_language(const char *fname)
{
	const u32 lang = hash(fname);

	if (g_lang == lang)
		return true;

	if (lang == g_lang_default) {
		g_lang = g_lang_default;
		return true;
	}

	u32 new_lang = g_lang_default;

	FILE *fp = fopen(fname, "r");
	if (!fp) {
		log_error("failed to open language file '%s'", fname);
		return false;
	}

	memclr(g_localized_strings);
	memclr(g_localized_string_memory);

	u32 num_strings = 0;
	if (!vson_read_u32(fp, "num_strings", &num_strings)) {
		log_error("failed to read language.num_strings");
		goto out;
	}
	log_debug("localization table will use %u/%u slots",
	          num_strings, LOCALIZE_MAX_STR_SLOTS);
	if (num_strings > LOCALIZE_MAX_STR_SLOTS) {
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

		localized_string_t *string = localize__find_slot(id);
		if (string->id != 0)
			log_warn("duplicate string entry '%u'", id);
		string->id = id;

		if (!vson_read_str(fp, "str", &g_localized_string_memory[next_index],
		                   LOCALIZE_MAX_STR_BYTES-1-next_index)) {
			log_error("failed to read language.str #%u", i);
			goto out;
		}

		const u32 len = strlen(&g_localized_string_memory[next_index]);
		string->index = next_index;
		g_localized_string_memory[string->index+len] = 0;
		next_index += len + 1;

		if (next_index >= LOCALIZE_MAX_STR_BYTES) {
			log_error("too many language string bytes");
			goto out;
		}
	}

	log_debug("localization table will use %u/%u bytes",
	          next_index, LOCALIZE_MAX_STR_BYTES);

	new_lang = lang;

out:
	g_lang = new_lang;
	fclose(fp);
	return g_lang == lang;
}

b32 localize_save_language(const char *fname)
{
	FILE *fp = fopen(fname, "w");
	if (!fp)
		return false;

	u32 num_strings = 0;
	for (u32 i = 0; i < countof(g_localized_strings); ++i)
		if (g_localized_strings[i].id != 0)
			++num_strings;

	vson_write_u32(fp, "num_strings", num_strings);
	for (u32 i = 0; i < countof(g_localized_strings); ++i) {
		const localized_string_t *string = &g_localized_strings[i];
		vson_write_u32(fp, "id", string->id);
		vson_write_str(fp, "str", &g_localized_string_memory[string->index]);
	}

	fclose(fp);
	return true;
}

#endif // NO_LOCALIZE
#undef LOCALIZE_IMPLEMENTATION
#endif // LOCALIZE_IMPLEMENTATION
