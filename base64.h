#ifndef VIOLET_BASE64_H
#define VIOLET_BASE64_H

#include <stdlib.h>

size_t base64_encode_size(size_t data_size);
void   base64_encode(const void *data, size_t size, char *out);

size_t base64_decode_size(const char *data, size_t data_size);
void   base64_decode(const char *data, size_t size, void *out);

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, const char *argv[])
{
	if (argc > 1) {
		const char   *in       = argv[1];
		const size_t  in_size  = strlen(in);
		const size_t  out_size = base64_encode_size(in_size);
		char         *out      = malloc(out_size + 1);
		base64_encode(in, in_size, out);
		out[out_size] = 0;
		printf("%s\n", out);

		const size_t  re_size  = base64_decode_size(out, out_size);
		char         *re       = malloc(re_size + 1);
		base64_decode(out, out_size, re);
		re[re_size] = 0;
		printf("%s\n", re);
		return 0;
	}
	return 1;
}
#endif

#endif // VIOLET_BASE64_H


/* Implementation */

#ifdef BASE64_IMPLEMENTATION

#include <string.h>

size_t base64_encode_size(size_t data_size)
{
	const size_t raw_size = (data_size * 4) / 3;
	return ((raw_size + 3) / 4) * 4; /* round up to nearest multiple of 4 */
}

#define min(x, y) (((x) > (y)) ? (y) : (x))

static inline
void base64__copy_triplet(const char *data, size_t size, char buffer[3])
{
	memset(buffer, 0, 3 * sizeof(buffer[0]));
	memcpy(buffer, data, min(size, 3));
}

static inline
void base64__encode_triplet(char *pout, const char buffer[3])
{
	static const char table[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
	};

	pout[0] = table[(buffer[0] & 0xfc) >> 2];
	pout[1] = table[((buffer[0] & 0x03) << 4) | ((buffer[1] & 0xf0) >> 4)];
	pout[2] = table[((buffer[1] & 0x0f) << 2) | ((buffer[2] & 0xc0) >> 6)];
	pout[3] = table[(buffer[2] & 0x3f)];
}

void base64_encode(const void *data, size_t size, char *out)
{
	const char *pdata = data;
	const char *end = pdata + size;
	size_t remaining = size;
	char *pout = out;
	char buffer[3];

	while (pdata < end) {
		base64__copy_triplet(pdata, remaining, buffer);
		base64__encode_triplet(pout, buffer);
		pdata     += 3;
		remaining -= 3;
		pout      += 4;
	}

	if (pdata > end + 1)
		pout[-2] = '=';
	if (pdata > end)
		pout[-1] = '=';
}

size_t base64_decode_size(const char *data, size_t data_size)
{
	const size_t raw_size = (data_size / 4) * 3;
	return raw_size - (data[data_size-1] == '=') - (data[data_size-2] == '=');
}

static inline
void base64__decode_quartet(const char data[4], char *pout)
{
	static const char table[128] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,
		 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
		 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0,
	};

	/* gcc warns about using char as an array subscript */
	const char s[4] = {
		table[(unsigned char)data[0]],
		table[(unsigned char)data[1]],
		table[(unsigned char)data[2]],
		table[(unsigned char)data[3]],
	};

	pout[0] = (s[0] << 2) | ((s[1] & 0x30) >> 4);
	pout[1] = ((s[1] & 0x0f) << 4) | ((s[2] & 0x3c) >> 2);
	pout[2] = ((s[2] & 0x03) << 6) | s[3];
}

void base64_decode(const char *data, size_t size, void *out)
{
	const char *pdata = data;
	const char *end = pdata + size;
	char       *pout = out;

	while (pdata < end) {
		base64__decode_quartet(pdata, pout);
		pdata += 4;
		pout  += 3;
	}
}

#undef BASE64_IMPLEMENTATION
#endif // BASE64_IMPLEMENTATION
