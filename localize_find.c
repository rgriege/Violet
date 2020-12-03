#include <ctype.h>

#define CORE_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#define OS_IMPLEMENTATION
#include "violet/core.h"
#include "violet/array.h"
#include "violet/string.h"
#include "violet/os.h"

static
char *skip_whitespace(const char *str)
{
	char *p = (char*)str;
	while (isspace(*p)) ++p;
	return p;
}

static
void find_strings(char *contents, const char *prefix)
{
	assert(strlen(prefix) != 0);
	assert(prefix[strlen(prefix)-1] == '(');

	char *p = contents;
	const size_t prefix_size = strlen(prefix);

	while ((p = strstr(p, prefix))) {
		p += prefix_size;
		p = skip_whitespace(p);
		if (*p != '"') continue;
		++p;
		b32 inside_quotes = true;
		while (*p != 0 && !(*p == ')' && !inside_quotes)) {
			if (*p == '"' && p[-1] != '\\')
				inside_quotes = !inside_quotes;
			else if (inside_quotes)
				fputc(*p, stdout);
			++p;
		}
		fputc('\n', stdout);
	}
}

int main(int argc, char *const argv[])
{
	if (argc < 2) {
		printf("Usage: localize <FILE>\n");
		return 1;
	}

	for (int i = 1; i < argc; ++i) {
		size_t sz;
		void *contents = file_read_all(argv[i], "r", &sz, g_allocator);
		if (!contents) {
			printf("Failed to read file '%s'\n", argv[i]);
			return 1;
		}

		find_strings(contents, "LOCALIZE(");
		find_strings(contents, "LOCALIZE_STATIC(");

		afree(contents, g_allocator);
	}
	return 0;
}
