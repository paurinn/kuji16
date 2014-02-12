/*
Kuji16 Flash MCU Programmer
Copyright (C) 2014 Kari Sigurjonsson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
@addtogroup util
@{
*/
#include "stdafx.h"

int hex_dump(FILE *F, void *data, int size) {
	/* dumps size bytes of *data to stdout. Looks like:
	 * [00000000]  75 6E 6B 6E 6F 77 6E 20 30  FF 00 00 00 00 39 00  unknown 0.....9.
	 */

	unsigned char *p = data;
	unsigned char c;
	int n;
	int oc = 0;
	char bytestr[4] = {0};
	char addrstr[10] = {0};
	char hexstr [16*3 + 5] = {0};
	char charstr[16*1 + 5] = {0};

	if (F == NULL) { F = stdout; }

	for(n = 1; n <= size; n++) {
		if (n % 16 == 1) {
			/* store address for this line */
			snprintf(addrstr, sizeof(addrstr), "%08X", (void *)p - data);
		}

		c = *p;
		if (isalnum(c) == 0 && isgraph(c) == 0) {
			c = '.';
		}

		/* store hex str (for left side) */
		snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
		strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr) - 1);

		/* store char str (for right side) */
		snprintf(bytestr, sizeof(bytestr), "%c", c);
		strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr) - 1);

		if (n % 16 == 0) {
			/* line completed */
			oc += fprintf(F, "[%8.8s]  %-50.50s |%-16.16s|\n", addrstr, hexstr, charstr);
			hexstr[0] = 0;
			charstr[0] = 0;
		} else if (n % 8 == 0) {
			/* half line: add whitespaces */
			strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr) - 1);
			strncat(charstr, "  ", sizeof(charstr)-strlen(charstr) - 1);
		}
		p++; /* next byte */
	}

	if (strlen(hexstr) > 0) {
		/* print rest of buffer if not empty */
		oc += fprintf(F, "[%8.8s]  %-50.50s |%-16.16s|\n", addrstr, hexstr, charstr);
	}

	return oc;
}

double get_ticks() {
#ifdef __WIN32__
	return (double)GetTickCount() / (double)1000.0;
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + ((double)ts.tv_nsec / (double)1000000000.0);
#endif
}

void msleep(uint32_t ms) {
#ifdef __WIN32__
	SleepEx(ms, TRUE);
#else
	struct timespec ts = { .tv_sec = 0, .tv_nsec = CLAMP((ms * 1000000), 0, 999999999) };
	nanosleep(&ts, NULL);
#endif
}

#ifndef ntohll
uint64_t ntohll(const uint64_t input) {
	uint64_t rval;
	uint8_t *data = (uint8_t *)&rval;

	data[0] = input >> 56;
	data[1] = input >> 48;
	data[2] = input >> 40;
	data[3] = input >> 32;
	data[4] = input >> 24;
	data[5] = input >> 16;
	data[6] = input >> 8;
	data[7] = input >> 0;

	return rval;
}
#endif

#ifndef htonll
uint64_t htonll(const uint64_t input) {
	return (ntohll(input));
}
#endif

uint32_t string_hash(const char *str, int32_t len) {
	uint32_t hash = 5381;
	int32_t c;

	if (str == NULL) { return 0; }

	if (len <= 0) {
	    len = strlen(str);
	}

	while ((c = *(str++)) && len--) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

char *str_ltrim(char *str) {
	char *start;
	char *s;
	start = s = str;

	// Check if there are white spaces
	while (*str) {
		if (!isspace(*str)) {
			break;
		}
		str++;
	}

	// Move string start if there are white spaces
	if (start != str) {
		while (*str) {
			*s++ = *str++;
		}
		*s++ = *str++;
	}

	return(start);
}

char *str_rtrim(char *str) {
	int len;

	len = strlen(str) - 1;
	while (*str) {
		if (!isspace(str[len])) {
			break;
		}
		str[len--] = '\0';
	}
	return(str);
}

char *str_trim(char *str) {
	return(str_rtrim(str_ltrim(str)));
}

void str_reverse(char *s) {
    int c, i, j;
    for ( i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int32_t strtoint32(char *s, int base, int *status) {
	int32_t val;
	char *endptr;

	if (s == NULL) {
		if (status) *status = E_ARGUMENT;
		return 0;
	}

	errno = 0;
	val = strtol(s, &endptr, base);

	if (errno != 0) {
		if (status) *status = E_CONVERT;
		return 0;
	}

	if (endptr == s) {
		if (status) *status = E_EMPTY;
		return 0;
	}

	if (status) *status = E_NONE;
	return val;
}

int64_t strtoint64(char *s, int base, int *status) {
	int64_t val;
	char *endptr;

	if (s == NULL) {
		if (status) *status = E_ARGUMENT;
		return 0;
	}

	errno = 0;
	val = strtoll(s, &endptr, base);

	if (errno != 0) {
		if (status) *status = E_CONVERT;
		return 0;
	}

	if (endptr == s) {
		if (status) *status = E_EMPTY;
		return 0;
	}

	if (status) *status = E_NONE;
	return val;
}

float strtofloat(char *s, int *status) {
	float val;
	char *endptr;

	if (s == NULL) {
		if (status) *status = E_ARGUMENT;
		return 0;
	}

	errno = 0;
	val = strtof(s, &endptr);

	//Check for various possible errors.
	if ((errno == ERANGE && (val == HUGE_VALF || val == HUGE_VALL)) || (errno != 0 && val == 0.0)) {
		if (status) *status = E_CONVERT;
		return 0;
	}

	if (endptr == s) {
		if (status) *status = E_EMPTY;
		return 0;
	}

	//Success.
	if (status) *status = E_NONE;
	return val;
}

double strtodouble(char *s, int *status) {
	double val;
	char *endptr;

	if (s == NULL) {
		if (status) *status = E_ARGUMENT;
		return 0;
	}

	errno = 0;
	val = strtod(s, &endptr);

	//Check for various possible errors.
	if ((errno == ERANGE && (val == HUGE_VALF || val == HUGE_VALL)) || (errno != 0 && val == 0.0)) {
		if (status) *status = E_CONVERT;
		return 0;
	}

	if (endptr == s) {
		if (status) *status = E_EMPTY;
		return 0;
	}

	//Success.
	if (status) *status = E_NONE;
	return val;
}

#ifndef HAVE_ASPRINTF
int vasprintf(char **ret, const char *format, va_list args) {
	va_list copy;
	va_copy(copy, args);

	/* Make sure it is determinate, despite manuals indicating otherwise */
	*ret = 0;

	int count = vsnprintf(NULL, 0, format, args);
	if (count >= 0) {
		char *buffer = (char *)malloc(count + 1);
		if (buffer != NULL) {
			count = vsnprintf(buffer, count + 1, format, copy);
			if (count < 0)
			free(buffer);
			else
			*ret = buffer;
		}
	}
	va_end(copy);  // Each va_start() or va_copy() needs a va_end()

	return count;
}

int asprintf(char **ret, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int count = vasprintf(ret, format, args);
	va_end(args);
	return(count);
}
#endif

int timestamp(char *dest) {
	int n;

#ifdef __WIN32__
	struct _SYSTEMTIME st;
	GetSystemTime(&st);
	n = sprintf(dest, "%02d.%02d.%04d %02d:%02d:%02d+%02d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
	n = sprintf(dest, "%.6f", get_ticks());
#endif

	return n;
}

char *daystamp(char *dest) {
#ifdef __WIN32__
	struct _SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(dest, "%02d.%02d.%04d", st.wDay, st.wMonth, st.wYear);
#else
	sprintf(dest, "%.6f", get_ticks());
#endif

	return dest;
}

int hexgroup(uint8_t *data, int size, char *out, int outsize, ...) {
	int n, d, b;
	uint8_t *p;
	va_list ap;

	n = 0;
	p = data;
	va_start(ap, outsize);
	d = va_arg(ap, int);
	while (d > 0) {
		out[n++] = '[';
		for (b = 0; b < d; b++) {
			n += snprintf(out + n, outsize - n, "%02X", *p);
			p++;
			if ((p - data) > size || n >= outsize) break;
		}
		out[n++] = ']';
		d = va_arg(ap, int);
	}
	va_end(ap);

	return n;
}

int strsplit(char *str, char *delim, strsplit_t callback, void *argument) {
	char *saveptr = str, *seg;
	int i = 0;

	assert(str);
	assert(delim);

	for (i = 0; ; i++, str = NULL) {
		seg = strtok_r(str, delim, &saveptr);
		if (seg == NULL) break;
		callback(seg, argument);
	}
	return i;
}

#ifndef strtok_r
char *strtok_r(char *str, const char *delim, char **nextp) {
	char *ret;

	if (str == NULL) {
		str = *nextp;
	}

	str += strspn(str, delim);

	if (*str == '\0') {
		return NULL;
	}

	ret = str;

	str += strcspn(str, delim);

	if (*str) {
		*str++ = '\0';
	}

	*nextp = str;

	return ret;
}
#endif

int fwithin_p(float value, float limit, float deviation) {
	return (value >= limit - (limit * deviation) && value <= limit + (limit * deviation));
}

int filter_new(struct filter **filter, size_t size, int32_t error) {
	calloc(1, sizeof(struct filter));
	assert(filter);
	(*filter)->error = error;
	return E_NONE;
}

int64_t filter_push(struct filter *filter, int32_t value) {
	int64_t sum = 0;

	if (llabs(filter->mean - value) <= filter->error) {
		filter->stablecount++;
		filter->stable = filter->stablecount > ARRAY_SIZE(filter->values);
	} else {
		filter->stablecount = 0;
		filter->stable = false;
	}

	filter->values[filter->index++ % ARRAY_SIZE(filter->values)] = value;
	for (size_t i = 0; i < ARRAY_SIZE(filter->values); i++) {
		sum += filter->values[i];
	}

	filter->mean = sum / ARRAY_SIZE(filter->values);

	return filter->mean;
}

void filter_free(struct filter **filter) {
	if (filter && *filter) {
		free(*filter);
		*filter = NULL;
	}
}

void filter_zero(struct filter *filter, uint32_t error) {
	memset(filter->values, 0x00, sizeof(filter->values));
	filter->mean = 0;
	filter->stablecount = 0;
	filter->stable = false;
	filter->error = error > 0 ? error : 20;
}

/********************************[ZLIB CODE]**********************************

	Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

	Jean-loup Gailly        Mark Adler
	jloup@gzip.org          madler@alumni.caltech.edu

*/

/** Table of CRCs of all 8-bit messages. */
static unsigned long crc16_table[256];

/** Flag: has the table been computed? Initially false. */
static int crc16_table_computed = 0;

/** Calculate CRC16  values for all 8-bit messages. */
void make_crc16_table(void) {
	unsigned long c;
	int n, k;
	for (n = 0; n < 256; n++) {
		c = (unsigned long) n;
		for (k = 0; k < 8; k++) {
			if (c & 1) {
				c = 0xedb88320L ^ (c >> 1);
			} else {
				c = c >> 1;
			}
		}
		crc16_table[n] = c;
	}
	crc16_table_computed = 1;
}

unsigned long update_crc16(unsigned long crc, unsigned char *buf, int len) {
	unsigned long c = crc ^ 0xffffffffL;
	int n;
	if (!crc16_table_computed) make_crc16_table();
	for (n = 0; n < len; n++) c = crc16_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	return c ^ 0xffffffffL;
}

unsigned long crc16(unsigned char *buf, int len) {
	return update_crc16(0L, buf, len);
}

/******************************[END OF ZLIB CODE]******************************/

uint16_t update_crc16x(uint16_t crc, uint8_t *buf, uint16_t len) {
	while (len > 0) {
		crc = ((crc << 5) | (crc >> 11)) ^ *buf;
		len--;
		buf++;
	}

	return crc;
}

uint16_t crc16x(uint8_t *buf, uint16_t len) {
	return update_crc16x(0, buf, len);
}

uint32_t crc24(uint16_t x16) {
	uint8_t b1, b2, b3;
	uint32_t res;

	x16 = ((x16 << 5) | (x16 >> 11));
	b1 = x16 & 0xFF;
	x16 = x16 ^ b1;
	res = b1;

	x16 = ((x16 << 5) | (x16 >> 11));
	b2 = x16 & 0xFF;
	x16 = x16 ^ b2;
	res = res | ((uint32_t)b2 << 8);

	x16 = ((x16 << 5) | (x16 >> 11));
	b3 = x16 & 0xFF;
	res = res | ((uint32_t)b3 << 16);

	return res;
}

uint8_t checksum(uint8_t *buf, int size) {
	uint16_t sum = 0;
	while (size > 0) {
		sum += *buf;
		size--;
		buf++;
	}

	return (sum & 0x00FF);
}

/**
	Update a crc16 value with some data.
	@param crc The current checksum.
	@param value Data to sum into crc.
	@return Returns the new crc value.
*/
unsigned int crcitt_update(unsigned int crc, unsigned int value);
inline unsigned int crcitt_update(unsigned int crc, unsigned int value) {
    value <<= 8;
    for (int i = 0; i < 8; i++) {
        if((crc ^ value) & 0x8000) crc = (crc << 1) ^ 0x1021; else crc <<= 1;
        value <<= 1;
    }
    return crc;
}

unsigned int crcitt(uint8_t *buf, int len) {
    unsigned int crc;
	for (crc = 0; len; len--, buf++) {
        crc = crcitt_update(crc, (unsigned int)*buf);
    }
    return crc;
}

long filedata(const char *path, uint8_t **buf) {
	FILE *F;
	long size;
	long nread;

	assert(path);
	assert(buf);

	F = fopen (path, "rb");
	if (F == NULL) {
		LOGE("Could not open '%s' for reading.", path);
		return E_OPEN;
	}

	// obtain file size:
	fseek(F, 0 , SEEK_END);
	size = ftell(F);
	fseek(F, 0 , SEEK_SET);

	*buf = (uint8_t *)calloc(1, sizeof(uint8_t) * size);
	assert(*buf);

	nread = fread(*buf, 1, size, F);
	fclose(F);

	if (nread <= 0) {
		free(*buf);
		*buf = NULL;
		return E_READ;
	}

	if (nread < size) {
		LOGW("Short read (%d < %d) from file '%s'.", nread, size, path);
	}

	return nread;
}

/**
	Scan into ipv4 string and see if it contains an IPv4 compatible address.
	Compatible address is either a decimal value or 4 dot seperated decimals (0-255).
	The string ipv4 may contain any isspace() valid characters before the address.
	@param ipv4 The string to scan.
	@return If there is a valid IPv4 adress in the string then we return it's starting character.
	@return Otherwise returns NULL.
*/
const char *validate_ipv4(const char *ipv4) {
	const char *s;
	int a, b, c, d, rc;
	//Skip past any whitespace at the beginning.
	for (s = ipv4; s && isspace(*s); s++);
	//Let generic pattern matcher do the heavy lifting.
	rc = sscanf(s, "%d.%d.%d.%d", &a, &b, &c, &d);
	//FIXME, if rc == 1 then divide ip to 4 bytes.
	if (rc != 4) return NULL;
	return ((a >= 0 && a <= 255) && (b >= 0 && b <= 255) && (c >= 0 && c <= 255) && (d >= 0 && d <= 255)) ? s : NULL;
}

/** @} */

