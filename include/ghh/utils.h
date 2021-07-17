#ifndef GHH_UTILS_H
#define GHH_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

// constants
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifndef M_SQRT2
#define M_SQRT2	1.4142135623730951
#endif

// common macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) (MAX(MIN(x, b), a))
#define INRANGE(x, a, b) ((x) >= (a) && (x) < (b))
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(*arr))

// loops
#define FOR_XY(x, y, mx, my) for (y = 0; y < my; ++y) for (x = 0; x < mx; ++x)
#define FOR_XYZ(x, y, z, mx, my, mz) for (z = 0; z < mz; ++z) for (y = 0; y < my; ++y) for (x = 0; x < mx; ++x)
#define FOR_CUBE(x, y, z, minv, maxv) for (z = minv; z < maxv; ++z) for (y = minv; y < maxv; ++y) for (x = minv; x < maxv; ++x)

// bitfield ops
#define BIT_GET(bitfield, index) ((bitfield >> (index)) & 1)
#define BIT_SET_TRUE(bitfield, index) {bitfield |= (0x1 << (index));}
#define BIT_SET_FALSE(bitfield, index) {bitfield &= ~(0x1 << (index));}

#define BIT_SET_COND(bitfield, index, cond) {\
	if (cond) \
		BIT_SET_TRUE(bitfield, index) \
	else \
		BIT_SET_FALSE(bitfield, index) \
}

void print_bits(const char *message, unsigned n, size_t bits);

// swap
#define SWAP(a, b, temp) {\
	temp = a;\
	a = b;\
	b = temp;\
}

// swp is bool/unsigned/int
#define BIT_SET_SWAP(bitfield, index1, index2, swp) {\
	swp = BIT_GET(bitfield, index1);\
	BIT_SET_COND(bitfield, index1, BIT_GET(bitfield, index2));\
	BIT_SET_COND(bitfield, index2, swp);\
}

// errors
#define ERROR(msg, ...) {\
	fprintf(stderr, "ERROR:%s:%d\n", __FILE__, __LINE__);\
	fprintf(stderr, __VA_ARGS__ /* error here because of comma? use ERROR0 */ );\
	exit(-1);\
}

#define ERROR0(msg) {\
	fprintf(stderr, "ERROR:%s:%d\n", __FILE__, __LINE__);\
	fprintf(stderr, msg);\
	exit(-1);\
}

// float stuff
#define EPSILON	0.000001

#define fround(d) (floor(d + 0.5 + EPSILON))

static inline bool fequals(double a, double b) {
	return fabs(a - b) < EPSILON;
}

static inline double randf() {
	return (double)rand() / (double)RAND_MAX;
}

static inline double rand_angle() {
	return randf() * M_PI * 2.0;
}

// time
void timeit_start(void);
void timeit_end(const char *message);
double timeit_get_time(void);

// terminal display
void term_set_bg(int color);
void term_set_fg(int color);
void term_reset_color();

#endif
