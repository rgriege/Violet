#ifndef VIOLET_STRING_H
#define VIOLET_STRING_H

/* C strings */
char *strtrim(char *str);
char *sprint_u32(char *buf, u32 n, u32 val);
char *sprint_s32(char *buf, u32 n, s32 val);
char *sprint_r32(char *buf, u32 n, r32 val, u32 dec);

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
char *imstrcat(const char *src);
char *imstrcatn(char *imstr, const char *src); /* for chaining imprint calls */
char *imstrcatp(const char *src, char *imstr); /* for chaining imprint calls */
char *imstrcat2(const char *src1, const char *src2); /* for 2 new strings */

/* Dynamic strings - array(char) */
#define str_t array(char)

#define str_create(allocator) array_create_ex(allocator)
#define str_destroy(str)      array_destroy(str)

str_t str_dup(const char *src, allocator_t *a);

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
	strncpy(g_imprint_buf, str, IMPRINT_BUFFER_SIZE);
	return g_imprint_buf;
}

char *imstrcat(const char *src)
{
	assert(strlen(imstr()) + strlen(src) < IMPRINT_BUFFER_SIZE);
	return strncat(imstr(), src, IMPRINT_BUFFER_SIZE-strlen(imstr())-1);
}

char *imstrcatn(char *imstr, const char *src)
{
	assert(imstr == g_imprint_buf);
	return imstrcat(src);
}

char *imstrcatp(const char *src, char *imstr)
{
	assert(imstr = g_imprint_buf);
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

char *imstrcat2(const char *src1, const char *src2)
{
	assert(strlen(src1) < IMPRINT_BUFFER_SIZE);
	imstrcpy(src1);
	return strncat(g_imprint_buf, src2, IMPRINT_BUFFER_SIZE-strlen(src1)-1);
}

str_t str_dup(const char *src, allocator_t *a)
{
	const size_t sz = strlen(src);
	str_t str;
	array_init_ex(str, sz+1, a);
	str_cpy(&str, src);
	return str;
}

void str_cpy(str_t *dst, const char *src)
{
	array_set_sz(*dst, (array_size_t)(strlen(src) + 1));
	strcpy(*dst, src);
}

void str_cat(str_t *dst, const char *src)
{
	const size_t sz = strlen(*dst);
	array_set_sz(*dst, (array_size_t)(sz + strlen(src) + 1));
	strcat(&(*dst)[sz], src);
}

void str_cat2(str_t *dst, const char *src1, const char *src2)
{
	const size_t dsz = strlen(*dst);
	const size_t sz1 = strlen(src1);
	const size_t sz2 = strlen(src2);
	array_set_sz(*dst, (array_size_t)(dsz + sz1 + sz2 + 1));
	strcat(&(*dst)[dsz], src1);
	strcat(&(*dst)[dsz+sz1], src2);
}

void str_catw(str_t *dst, const char *src)
{
	str_cat2(dst, " ", src);
}

#undef STRING_IMPLEMENTATION
#endif
