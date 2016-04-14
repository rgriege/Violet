#ifndef VIOLET_CSS_H
#define VIOLET_CSS_H

#include "violet/gui/color.h"

typedef struct vlt_css
{
	vlt_color color;
} vlt_css;

void vlt_css_parse(vlt_css * style, const char * txt);

#endif
