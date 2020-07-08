#ifndef VIOLET_STRING_H
#define VIOLET_STRING_H

/* C strings */
char *strtrim(char *str);
char *sprint_u32(char *buf, u32 n, u32 val);
char *sprint_s32(char *buf, u32 n, s32 val);
char *sprint_s64(char *buf, u32 n, s64 val);
char *sprint_r32(char *buf, u32 n, r32 val, u32 dec);

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
char* strncpy_nt(char* dst, const char* src, size_t size);
#define strbcpy(dst, src) strncpy_nt(dst, src, sizeof(dst))

/* Immediate strings - single string buffer for immediate use.
 * Be very careful when writing functions that take a string as a parameter
 * and use this string - assert(str != imstr()) is recommended.
 *
 * The default is long enough for most OS's max file path length. */
#ifndef IMPRINT_BUFFER_SIZE
#define IMPRINT_BUFFER_SIZE 4096
#endif

char *imstr(void);
char *imprint_u32(u32 val);
char *imprint_s32(s32 val);
char *imprint_r32(r32 val, u32 dec);
char *imprintf(const char *fmt, ...);
char *imstrcpy(const char *str);
char *imstrcpy2(const char *src1, const char *src2);
char *imstrcpyv(const char *src1, ...); /* final parameter must be NULL */
char *imstrcat(const char *src);
char *imstrcatv(const char *src1, ...); /* final parameter must be NULL */
char *imstrcatn(char *imstr, const char *src); /* for chaining imprint calls */
char *imstrcatnv(char *imstr, ...); /* final parameter must be NULL */
char *imstrcatp(const char *src, char *imstr); /* for chaining imprint calls */
char *imstrcatprintf(char *imstr, const char *fmt, ...);
char *imstrcatprintfv(char *imstr, const char *fmt, va_list args);

/*
 * Dynamic string
 * Always stores the null terminating character.
 */
#define str_t array(char)

str_t  str_create(allocator_t *a);
str_t  str_dup(const char *src, allocator_t *a);
void   str_destroy(str_t *str);

str_t *str_cpy(str_t *dst, const char *src);
str_t *str_cpy2(str_t *dst, const char *src1, const char *src2);
str_t *str_cat(str_t *dst, const char *src);

str_t *str_clear(str_t *str);
str_t *str_remove_to_end(str_t *str, const char *p);

char  *str_beg(str_t *str);
char  *str_end(str_t *str); // pointer to null terminator
size_t str_len(str_t *str); // like strlen, doesn't include null terminator

#define cstr(str) str_beg(str)

#endif // VIOLET_STRING_H

#ifdef STRING_IMPLEMENTATION

char *strtrim(char *str)
{
	const size_t sz = strlen(str);
	size_t n = 0;
	while (n < sz && str[n] == ' ')
		++n;
	for (size_t i = 0; i <= sz - n; ++i)
		str[i] = str[i+n];
	return str;
}

static
void sprint__separators(char *buf, size_t sep_off, size_t sep_cnt, size_t len)
{
	for (size_t i = 0; i < sep_cnt; ++i) {
		const size_t sep_pos = sep_off + i * 4;
		memmove(buf + sep_pos + 1, buf + sep_pos, len - sep_pos + 1);
		buf[sep_pos] = ',';
	}
}

static
void sprint__stats(const char *buf, u32 dec,
                   size_t *off, size_t *len,
                   size_t *sep_cnt, size_t *sep_off)
{
	*off = (buf[0] == '-');
	*len = strlen(buf);
	*sep_cnt = (*len - *off - 1) / 3;
	*sep_off = ((*len - *off - 1) % 3) + 1 + *off;
	*len += *sep_cnt + (dec > 0) + dec;

}

char *sprint_u32(char *buf, u32 n, u32 val)
{
	size_t off, len, sep_cnt, sep_off;

	snprintf(buf, n, "%u", val);
	sprint__stats(buf, 0, &off, &len, &sep_cnt, &sep_off);

	if (len >= n) {
		assert(false);
		goto out;
	}

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);
out:
	return buf;
}

char *sprint_s32(char *buf, u32 n, s32 val)
{
	size_t off, len, sep_cnt, sep_off;

	snprintf(buf, n, "%d", val);
	sprint__stats(buf, 0, &off, &len, &sep_cnt, &sep_off);

	if (len >= n) {
		assert(false);
		goto out;
	}

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);
out:
	return buf;
}

char *sprint_s64(char *buf, u32 n, s64 val)
{
	size_t off, len, sep_cnt, sep_off;

	snprintf(buf, n, "%" PRId64, val);
	sprint__stats(buf, 0, &off, &len, &sep_cnt, &sep_off);

	if (len >= n) {
		assert(false);
		goto out;
	}

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);
out:
	return buf;
}

char *sprint_r32(char *buf, u32 n, r32 val, u32 dec)
{
	size_t off, len, sep_cnt, sep_off;

	assert(dec >= 0 && dec < 9);

	snprintf(buf, n, "%.0f", val);
	sprint__stats(buf, dec, &off, &len, &sep_cnt, &sep_off);

	{
		char fmt[8] = "%.xf";
		fmt[2] = '0' + dec;
		snprintf(buf, n, fmt, val);
	}

	if (len >= n) {
		assert(false);
		goto out;
	}

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);

out:
	return buf;
}

char* strncpy_nt(char* dst, const char* src, size_t size)
{
#ifdef __EMSCRIPTEN__
	/* For some reason, the strncpy implementation throws runtime errors
	 * with sanitize flags enabled. */
	dst[0] = 0;
	strncat(dst, src, size-1);
	return dst;
#else
#if defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
	strncpy(dst, src, size);
	dst[size - 1] = '\0';
	return dst;
#if defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif
#endif // __EMSCRIPTEN__
}

static thread_local char g_imprint_buf[IMPRINT_BUFFER_SIZE] = {0};

char *imstr(void)
{
	return g_imprint_buf;
}

char *imprint_u32(u32 val)
{
	return sprint_u32(B2PC(g_imprint_buf), val);
}

char *imprint_s32(s32 val)
{
	return sprint_s32(B2PC(g_imprint_buf), val);
}

char *imprint_r32(r32 val, u32 dec)
{
	return sprint_r32(B2PC(g_imprint_buf), val, dec);
}

char *imprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(g_imprint_buf, IMPRINT_BUFFER_SIZE, fmt, args);
	va_end(args);
	return g_imprint_buf;
}

char *imstrcpy(const char *str)
{
	assert(strlen(str) < IMPRINT_BUFFER_SIZE);
	strbcpy(g_imprint_buf, str);
	return g_imprint_buf;
}

char *imstrcpy2(const char *src1, const char *src2)
{
	return imstrcatn(imstrcpy(src1), src2);
}

static
void imstrcatv_(va_list args)
{
	const char *str = va_arg(args, const char*);
	while (str) {
		imstrcat(str);
		str = va_arg(args, const char*);
	}
}

char *imstrcpyv(const char *src1, ...)
{
	va_list args;
	va_start(args, src1);
	imstrcpy(src1);
	imstrcatv_(args);
	va_end(args);
	return imstr();
}

char *imstrcat(const char *src)
{
	assert(strlen(imstr()) + strlen(src) < IMPRINT_BUFFER_SIZE);
	return strncat(imstr(), src, IMPRINT_BUFFER_SIZE-strlen(imstr())-1);
}

char *imstrcatv(const char *src1, ...)
{
	va_list args;
	va_start(args, src1);
	imstrcat(src1);
	imstrcatv_(args);
	va_end(args);
	return imstr();
}

char *imstrcatn(char *imstr, const char *src)
{
	assert(imstr == g_imprint_buf);
	return imstrcat(src);
}

char *imstrcatnv(char *imstr, ...)
{
	va_list args;
	va_start(args, imstr);
	imstrcatv_(args);
	va_end(args);
	return imstr;
}

char *imstrcatp(const char *src, char *imstr)
{
	assert(imstr == g_imprint_buf);
	const size_t srclen = strlen(src);
	const size_t imlen  = strlen(imstr);

	if (srclen + imlen < IMPRINT_BUFFER_SIZE) {
		memmove(g_imprint_buf + srclen, g_imprint_buf, imlen+1);
		memcpy(g_imprint_buf, src, srclen);
		return g_imprint_buf;
	} else {
		assert(false);
		return imstrcpy("");
	}
}

char *imstrcatprintf(char *imstr, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	imstrcatprintfv(imstr, fmt, args);
	va_end(args);
	return imstr;
}

char *imstrcatprintfv(char *imstr, const char *fmt, va_list args)
{
	const size_t sz = strlen(imstr);
	vsnprintf(g_imprint_buf + sz, IMPRINT_BUFFER_SIZE - sz, fmt, args);
	return imstr;
}

str_t str_create(allocator_t *a)
{
	return str_dup("", a);
}

str_t str_dup(const char *src, allocator_t *a)
{
	const size_t sz = strlen(src) + 1;
	str_t str;
	array_init_ex(str, (array_size_t)sz, a);
	str_cpy(&str, src);
	return str;
}

void str_destroy(str_t *str)
{
	array_destroy(*str);
	*str = NULL;
}

str_t *str_cpy(str_t *dst, const char *src)
{
	const size_t sz = strlen(src) + 1;
	array_clear(*dst);
	array_appendn(*dst, src, (array_size_t)sz);
	return dst;
}

str_t *str_cpy2(str_t *dst, const char *src1, const char *src2)
{
	str_cpy(dst, src1);
	str_cat(dst, src2);
	return dst;
}

str_t *str_cat(str_t *dst, const char *src)
{
	const size_t sz = strlen(src) + 1;
	array_pop(*dst);
	array_appendn(*dst, src, (array_size_t)sz);
	return dst;
}

str_t *str_clear(str_t *str)
{
	array_set_sz(*str, 1);
	array_last(*str) = 0;
	return str;
}

str_t *str_remove_to_end(str_t *str, const char *p)
{
	const size_t sz = p - str_beg(str) + 1;
	array_set_sz(*str, (array_size_t)sz);
	array_last(*str) = 0;
	return str;
}

char *str_beg(str_t *str)
{
	return (char*)*str;
}

char *str_end(str_t *str)
{
	return (char*)&array_last(*str);
}

size_t str_len(str_t *str)
{
	return array_sz(*str) - 1;
}

#undef STRING_IMPLEMENTATION
#endif // STRING_IMPLEMENTATION
