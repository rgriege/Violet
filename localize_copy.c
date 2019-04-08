#include <ctype.h>

#define LOCALIZE_MAX_STR_SLOTS 512
#define LOCALIZE_MAX_STR_BYTES 4096

#define CORE_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#define OS_IMPLEMENTATION
#define VSON_IMPLEMENTATION
#define LOCALIZE_IMPLEMENTATION
#include "violet/core.h"
#include "violet/array.h"
#include "violet/string.h"
#include "violet/os.h"
#include "violet/vson.h"
#include "violet/localize.h"


static void usage(void)
{
	printf("Usage: localize_copy [-g] <SOURCE_LANGUAGE_FILE> <DEST_LANGUAGE_FILE(S)>\n");
}

static char rand_char(void)
{
	static const char s_chars[] = { "abcdefghijklmnopqrstuvwxyz" };
	return s_chars[rand() % (countof(s_chars)-1)];
}

static b32 isformatspecifier(char c)
{
	/* %, c, s, d, i, o, x, X, u, f, F, e, E, a, A, g, G, n, p */
	static const b32 s_chars_format_specifier[128] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
		0,1,0,1,1,1,1,1,0,1,0,0,0,0,1,1,
		1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,
	};
	return c >= 0 && s_chars_format_specifier[(u8)c];
}

static void randomize_string(char *str)
{
	const size_t len = strlen(str);
	char last = ' ';
	for (size_t i = 0; i < len; ++i) {
		char c = str[i];
		if (str[i] == '%') {
			if (i > 0 && last == ' ')
				str[i-1] = ' ';
			do {
				++i;
			} while (i < len && !isformatspecifier(str[i]) && str[i] != 0);
			++i;
			if (i < len && str[i] == ' ')
				++i;
		} else {
			str[i] = rand_char();
		}
		last = c;
	}
}

int main(int argc, char *const argv[])
{
	b32 gibberish = false;
	localization_table_t src, dst;
	localized_string_t *slot;
	int i = 1;

	if (argc < 3) {
		usage();
		return 1;
	}

	if (strcmp(argv[i], "-g") == 0) {
		gibberish = true;
		srand(time(NULL));
		++i;
		if (argc < 4) {
			usage();
			return 1;
		}
	}

	log_add_stream(LOG_ALL, file_logger, stdout);

	memclr(src);
	if (!localization_table_load(&src, argv[i])) {
		printf("Failed to read source language file '%s'\n", argv[i]);
		return 1;
	}

	for (++i; i < argc; ++i) {
		memclr(dst);
		if (!localization_table_load(&dst, argv[i])) {
			printf("Failed to read file '%s'\n", argv[i]);
			continue;
		}

		/* clear strings in dst not in src */
		for (u32 j = 0; j < countof(dst.slots); ++j)
			if (   dst.slots[j].id != 0
			    && (   !localization_table_find_slot(&src, dst.slots[j].id, &slot)
			        || slot->id == 0))
				dst.slots[j].id = 0;

		u32 next_index = localization_table_num_bytes(&dst);

		/* add missing strings to dst */
		for (u32 j = 0; j < countof(src.slots); ++j) {
			if (src.slots[j].id != 0) {
				const char *str = &src.chars[src.slots[j].index];
				if (!localization_table_find_slot(&dst, src.slots[j].id, &slot)) {
					printf("Failed to find slot for %s in %s\n", str, argv[i]);
				} else if (slot->id != 0) {
					/* already exists, nothing to do */
				} else if (next_index + strlen(str) >= countof(dst.chars)) {
					printf("Failed to find memory for %s in %s\n", str, argv[i]);
				} else {
					slot->id = src.slots[j].id;
					slot->index = next_index;
					strcpy(&dst.chars[slot->index], str);
					if (gibberish)
						randomize_string(&dst.chars[slot->index]);
					next_index += strlen(str) + 1;
				}
			}
		}

		if (!localization_table_save(&dst, argv[i]))
			printf("Failed to write file '%s'\n", argv[i]);
	}
	return 0;
}
