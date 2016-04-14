#include <string.h>

#include "violet/serialization/css.h"
#include "violet/utility/log.h"

void vlt_css_parse(vlt_css * style, const char * txt)
{
	const char * p = txt;
	do
	{
		char label[20] = {0};
		char * l = label;
		while (*p != ':')
			*l++ = *p++;
		++p;

		char value[20] = {0};
		char * v = value;
		while (*p != ',' && *p != ';')
			*v++ = *p++;
		++p;

		if (strcmp(label, "fill") == 0)
			vlt_color_from_hex(&style->color, value);
		else
			log_write("invalid/unsupported css label '%s'", label);
	}
	while (*(p-1) != ';');
}

