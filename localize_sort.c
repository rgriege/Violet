#include <ctype.h>

#define LOCALIZE_MAX_STR_SLOTS 512
#define LOCALIZE_MAX_STR_BYTES 8192

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
	printf("Usage: localize_sort <LANGUAGE_FILE>\n");
}

int main(int argc, char *const argv[])
{
	const char *fname;
	localization_table_t table;

	if (argc != 2) {
		usage();
		return 1;
	}

	fname = argv[1];

	log_add_stream(LOG_ALL, file_logger, stdout);

	memclr(table);
	if (!localization_table_load(&table, fname)) {
		printf("Failed to read language file '%s'\n", fname);
		return 1;
	}

	localization_table_sort(&table);

	if (!localization_table_save(&table, fname)) {
		printf("Failed to write file '%s'\n", fname);
		return 1;
	}

	return 0;
}
