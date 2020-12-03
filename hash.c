#define CORE_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#include "violet/core.h"
#include "violet/array.h"
#include "violet/string.h"

int main(int argc, char *const argv[])
{
	str_t str = str_create(g_allocator);

	if (argc > 1)
		str_cat(&str, argv[1]);

	for (int i = 2; i < argc; ++i) {
		str_cat(&str, " ");
		str_cat(&str, argv[i]);
	}

	printf("%u\n", hash_compute(str));
	return 0;
}
