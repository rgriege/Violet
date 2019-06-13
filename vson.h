#ifndef VIOLET_VSON_H
#define VIOLET_VSON_H

#include "violet/core.h"

#ifndef VSON_LABEL_SZ
#define VSON_LABEL_SZ 64
#endif

#ifndef VSON_VALUE_SZ
#define VSON_VALUE_SZ 64
#endif

b32  vson_read_header(FILE *fp, const char *label);
b32  vson_read_b8(FILE *fp, const char *label, b8 *val);
b32  vson_read_u8(FILE *fp, const char *label, u8 *val);
b32  vson_read_s8(FILE *fp, const char *label, s8 *val);
b32  vson_read_char(FILE *fp, const char *label, char *val);
b32  vson_read_u16(FILE *fp, const char *label, u16 *val);
b32  vson_read_s16(FILE *fp, const char *label, s16 *val);
b32  vson_read_b32(FILE *fp, const char *label, b32 *val);
b32  vson_read_s32(FILE *fp, const char *label, s32 *val);
b32  vson_read_u32(FILE *fp, const char *label, u32 *val);
b32  vson_read_r32(FILE *fp, const char *label, r32 *val);
b32  vson_read_r64(FILE *fp, const char *label, r64 *val);
b32  vson_read_str(FILE *fp, const char *label, char *val, u32 sz);
void vson_write_header(FILE *fp, const char *label);
void vson_write_b32(FILE *fp, const char *label, b32 val);
void vson_write_s32(FILE *fp, const char *label, s32 val);
void vson_write_u32(FILE *fp, const char *label, u32 val);
void vson_write_r32(FILE *fp, const char *label, r32 val);
void vson_write_r64(FILE *fp, const char *label, r64 val);
void vson_write_char(FILE *fp, const char *label, char val);
void vson_write_str(FILE *fp, const char *label, const char *val);

#endif


/* Implementation */

#ifdef VSON_IMPLEMENTATION

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static b32 vson__read_label(FILE *fp, const char *label)
{
	b32 retval = false;
	char c = 0;
	char label_buf[VSON_LABEL_SZ], *bufp = label_buf;
	while ((c = fgetc(fp)) != EOF && isspace((u8)c))
		;
	if (isspace((u8)c)) {
		log_error("vson: failed reading label %s", label);
		goto out;
	}

	*(bufp++) = c;

	while (bufp - label_buf < VSON_LABEL_SZ && (c = fgetc(fp)) != EOF && c != ':')
		*(bufp++) = c;
	if (c != ':') {
		log_error("vson: missing colon after label %s", label);
		goto out;
	}

	*bufp = '\0';
	if (strcmp(label_buf, label) != 0) {
		log_error("vson: expected %s, got %s", label, label_buf);
		goto out;
	}

	retval = fgetc(fp) == ' ';
out:
	return retval;
}

static b32 vson__skip_rest_of_line(FILE *fp)
{
	char c;
	while ((c = fgetc(fp)) != EOF && c != '\n')
		;
	return c == '\n';
}

static u32 vson__read_rest_of_line(FILE *fp, char *str, u32 n)
{
	char c, *p = str;
	while ((u32)(p - str) < n && (c = fgetc(fp)) != EOF && c != '\n')
		*(p++) = c;
	if (c != '\n') {
		vson__skip_rest_of_line(fp);
		return 0;
	}
	if (p > str && p[-1] == '\r')
		p[-1] = '\0';
	else
		*p = '\0';
	return p - str;
}

#define VSON_READ_VAL(expr) \
	char buf[VSON_VALUE_SZ]; \
	if (!vson__read_label(fp, label)) { \
		vson__skip_rest_of_line(fp); \
		return false; \
	} \
	if (vson__read_rest_of_line(fp, buf, VSON_VALUE_SZ) == 0) \
		return false; \
	*val = expr; \
	return true

b32 vson_read_header(FILE *fp, const char *label)
{
	const b32 ret = vson__read_label(fp, label);
	vson__skip_rest_of_line(fp);
	return ret;
}

b32 vson_read_b8(FILE *fp, const char *label, b8 *val)
{
	b32 val_;
	if (vson_read_b32(fp, label, &val_)) {
		*val = val_;
		return true;
	}
	return false;
}

b32 vson_read_u8(FILE *fp, const char *label, u8 *val)
{
	u32 val_;
	if (vson_read_u32(fp, label, &val_) && val_ <= UINT8_MAX) {
		*val = val_;
		return true;
	}
	return false;
}

b32 vson_read_s8(FILE *fp, const char *label, s8 *val)
{
	s32 val_;
	if (vson_read_s32(fp, label, &val_) && val_ >= INT8_MIN && val_ <= INT8_MAX) {
		*val = val_;
		return true;
	}
	return false;
}

b32 vson_read_char(FILE *fp, const char *label, char *val)
{
	VSON_READ_VAL(buf[0]);
}

b32 vson_read_u16(FILE *fp, const char *label, u16 *val)
{
	u32 val_;
	if (vson_read_u32(fp, label, &val_) && val_ <= UINT16_MAX) {
		*val = val_;
		return true;
	}
	return false;
}

b32 vson_read_s16(FILE *fp, const char *label, s16 *val)
{
	s32 val_;
	if (   vson_read_s32(fp, label, &val_)
	    && val_ >= INT16_MIN
	    && val_ <= INT16_MAX) {
		*val = val_;
		return true;
	}
	return false;
}

b32 vson_read_b32(FILE *fp, const char *label, b32 *val)
{
	VSON_READ_VAL(buf[0] != '0' && buf[0] != 'f' && buf[0] != ' ');
}

b32 vson_read_s32(FILE *fp, const char *label, s32 *val)
{
	VSON_READ_VAL(strtol(buf, NULL, 10));
}

b32 vson_read_u32(FILE *fp, const char *label, u32 *val)
{
	VSON_READ_VAL(strtoul(buf, NULL, 10));
}

b32 vson_read_r32(FILE *fp, const char *label, r32 *val)
{
	VSON_READ_VAL(strtof(buf, NULL));
}

b32 vson_read_r64(FILE *fp, const char *label, r64 *val)
{
	VSON_READ_VAL(strtod(buf, NULL));
}

b32 vson_read_str(FILE *fp, const char *label, char *val, u32 sz)
{
	char c = 0, *p = val;

	if (!vson__read_label(fp, label)) {
		vson__skip_rest_of_line(fp);
		return false;
	}

	while (p < sz + val && (c = fgetc(fp)) != EOF && c != '\n' && c != '\r')
		*(p++) = c;

	if (p < val + sz && (c == EOF || c == '\r' || c == '\n')) {
		*p = '\0';
		return true;
	} else {
		return false;
	}
}


void vson_write_header(FILE *fp, const char *label)
{
	fprintf(fp, "\n%s: \n", label);
}

void vson_write_b32(FILE *fp, const char *label, b32 val)
{
	fprintf(fp, "%s: %c\n", label, val ? 't' : 'f');
}

void vson_write_s32(FILE *fp, const char *label, s32 val)
{
	fprintf(fp, "%s: %d\n", label, val);
}

void vson_write_u32(FILE *fp, const char *label, u32 val)
{
	fprintf(fp, "%s: %u\n", label, val);
}

void vson_write_r32(FILE *fp, const char *label, r32 val)
{
	fprintf(fp, "%s: %f\n", label, val);
}

void vson_write_r64(FILE *fp, const char *label, r64 val)
{
	fprintf(fp, "%s: %f\n", label, val);
}

void vson_write_char(FILE *fp, const char *label, char val)
{
	fprintf(fp, "%s: %c\n", label, val);
}

void vson_write_str(FILE *fp, const char *label, const char *val)
{
	fprintf(fp, "%s: %s\n", label, val);
}

#undef VSON_IMPLEMENTATION
#endif // VSON_IMPLEMENTATION
