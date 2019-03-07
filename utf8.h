#ifndef VIOLET_UTF8_H
#define VIOLET_UTF8_H

s32 utf8_next_codepoint(const char *nptr, char **endptr);
s32 utf8_prev_codepoint(const char *nptr, char **beginptr);

#endif // VIOLET_UTF8_H

/* Implementation */

#ifdef UTF8_IMPLEMENTATION

s32 utf8_next_codepoint(const char *nptr, char **endptr)
{
	const char *p = nptr;
	if (*p == 0) {
		if (endptr) *endptr = (char*)nptr;
		return 0;
	} else if (!(p[0] & 0x80)) {
		if (endptr) *endptr = (char*)nptr + 1;
		return p[0];
	} else if (!(p[0] & 0x20)) {
		if (endptr) *endptr = (char*)nptr + 2;
		return ((p[0] & 0x1f) << 6)  | (p[1] & 0x3f);
	} else if (!(p[0] & 0x10)) {
		if (endptr) *endptr = (char*)nptr + 3;
		return ((p[0] & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | (p[2] & 0x3f);
	} else {
		if (endptr) *endptr = (char*)nptr + 4;
		return ((p[0] & 0x07) << 18) | ((p[1] & 0x3f) << 12)
		     | ((p[2] & 0x3f) << 6)  | (p[3] & 0x3f);
	}
}

s32 utf8_prev_codepoint(const char *nptr, char **beginptr)
{
	const char *p = nptr-1;
	if (!(*p & 0x80)) {
		if (beginptr) *beginptr = (char*)p;
		return *p;
	} else {
		while (!(*p & 0x40))
			--p;
		if (beginptr) *beginptr = (char*)p;
		return utf8_next_codepoint(p, NULL);
	}
}

#undef UTF8_IMPLEMENTATION
#endif // UTF8_IMPLEMENTATION
