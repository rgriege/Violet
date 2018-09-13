#ifndef VIOLET_STRING_H
#define VIOLET_STRING_H

/* C strings */
char *strtrim(char *str);
char *sprint_u32(char *buf, u32 n, u32 val);
char *sprint_s32(char *buf, u32 n, s32 val);
char *sprint_r32(char *buf, u32 n, r32 val, u32 dec);

/* Immediate strings - single string buffer for immediate use.
 * Be very careful when writing functions that take a string as a parameter
 * and use this string - assert(str != imstr()) is recommended. */
#ifndef IMPRINT_BUFFER_SIZE
#define IMPRINT_BUFFER_SIZE 128
#endif

char *imstr(void);
char *imprint_u32(u32 val);
char *imprint_s32(s32 val);
char *imprint_r32(r32 val, u32 dec);
char *imprintf(const char *fmt, ...);
char *imstrcpy(const char *str);
char *imstrcat(const char *src);
char *imstrcatn(char *imstr, const char *src); /* for chaining imprint calls */
char *imstrcat2(const char *src1, const char *src2); /* for 2 new strings */

/* Dynamic strings - array(char) */
#define str_t array(char)

#define str_create(allocator) array_create_ex(allocator)
#define str_destroy(str)      array_destroy(str)

void str_cpy(str_t *dst, const char *src);
void str_cat(str_t *dst, const char *src);
void str_cat2(str_t *dst, const char *src1, const char *src2);
void str_catw(str_t *dst, const char *src);

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
		memmove(buf + sep_pos + 1, buf + sep_pos, len + sep_cnt - sep_pos);
		buf[sep_pos] = ',';
	}
}

char *sprint_u32(char *buf, u32 n, u32 val)
{
	size_t len, sep_cnt, sep_off;

	snprintf(buf, n, "%u", val);
	len = strlen(buf);
	sep_cnt = (len - 1) / 3;
	sep_off = ((len - 1) % 3) + 1;

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);
	return buf;
}

char *sprint_s32(char *buf, u32 n, s32 val)
{
	size_t len, sep_cnt, sep_off;

	snprintf(buf, n, "%d", val);
	len = strlen(buf);
	sep_cnt = (len - 1) / 3;
	sep_off = ((len - 1) % 3) + 1;

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);
	return buf;
}

char *sprint_r32(char *buf, u32 n, r32 val, u32 dec)
{
	size_t len, sep_cnt, sep_off;

	assert(dec >= 0 && dec < 9);

	snprintf(buf, n, "%.0f", val);
	len = strlen(buf);
	sep_cnt = (len - 1) / 3;
	sep_off = ((len - 1) % 3) + 1;

	assert(len + sep_cnt < n);

	{
		char fmt[8] = "%.xf";
		fmt[2] = '0' + dec;
		snprintf(buf, n, fmt, val);
	}

	if (sep_cnt > 0)
		sprint__separators(buf, sep_off, sep_cnt, len);
	return buf;
}

static char g_imprint_buf[IMPRINT_BUFFER_SIZE] = {0};

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
	strncpy(g_imprint_buf, str, IMPRINT_BUFFER_SIZE);
	return g_imprint_buf;
}

char *imstrcat(const char *src)
{
	return strncat(imstr(), src, IMPRINT_BUFFER_SIZE-strlen(imstr())-1);
}

char *imstrcatn(char *imstr, const char *src)
{
	assert(imstr == g_imprint_buf);
	return imstrcat(src);
}

char *imstrcat2(const char *src1, const char *src2)
{
	assert(strlen(src1) < IMPRINT_BUFFER_SIZE);
	imstrcpy(src1);
	return strncat(g_imprint_buf, src2, IMPRINT_BUFFER_SIZE-strlen(src1)-1);
}

void str_cpy(str_t *dst, const char *src)
{
	array_reserve(*dst, strlen(src) + 1);
	strcpy(*dst, src);
}

void str_cat(str_t *dst, const char *src)
{
	const size_t sz = array_sz(*dst);
	array_reserve(*dst, sz + strlen(src) + 1);
	strcat(&(*dst)[sz], src);
}

void str_cat2(str_t *dst, const char *src1, const char *src2)
{
	const size_t dsz = array_sz(*dst);
	const size_t sz1 = strlen(src1);
	const size_t sz2 = strlen(src2);
	array_reserve(*dst, dsz + sz1 + sz2 + 1);
	strcat(&(*dst)[dsz], src1);
	strcat(&(*dst)[dsz+sz1], src2);
}

void str_catw(str_t *dst, const char *src)
{
	str_cat2(dst, " ", src);
}

#endif
