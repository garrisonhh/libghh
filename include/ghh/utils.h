#ifndef GHH_UTILS_H
#define GHH_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// constants
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifndef M_SQRT2
#define M_SQRT2	1.4142135623730951
#endif

#define EPSILON	0.000001

// common macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) (MAX(MIN(x, b), a))
#define LERP(x, a, b) ((a) + (x) * ((b) - (a)))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// bitfield ops
#define BIT_GET(bitfield, index) ((bitfield >> (index)) & 1)
#define BIT_SET(bitfield, index) do {bitfield |= (0x1 << (index));} while (0)
#define BIT_CLEAR(bitfield, index) do {bitfield &= ~(0x1 << (index));} while (0)

#define BIT_SET_COND(bitfield, index, cond)\
	do {\
		if (cond) BIT_SET(bitfield, index);\
		else BIT_CLEAR(bitfield, index);\
	} while (0)

inline void print_bits(const char *message, unsigned n, size_t bits) {
	if (message != NULL)
		printf("%s:\t", message);

	for (int i = bits - 1; i >= 0; --i)
		printf("%u", BIT_GET(n, i));

	putchar('\n');
}

// swap
#define SWAP(a, b, temp)\
	do {\
		temp = a;\
		a = b;\
		b = temp;\
	} while (0)

// cursed macro.
#define SWAP_XOR(a, b) (((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b)))

// swp is bool/unsigned/int
#define BIT_SET_SWAP(bitfield, index1, index2, swp)\
	do {\
		swp = BIT_GET(bitfield, index1);\
		BIT_SET_COND(bitfield, index1, BIT_GET(bitfield, index2));\
		BIT_SET_COND(bitfield, index2, swp);\
	} while (0)

// errors
#define ERROR(...)\
	do {\
		fprintf(stderr, "ERROR:%s:%d\n", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
		exit(-1);\
	} while (0)

#ifdef DEBUG
#define SANITY_CHECK(cond, ...) if (!(cond)) ERROR(__VA_ARGS__)
#else
#define SANITY_CHECK(...)
#endif

// float stuff
#define fround(d) (floor(d + 0.5 + EPSILON))
#define fequals(a, b) (fabs(a - b) < EPSILON)
#define randf() ((double)rand() / (double)RAND_MAX)
#define rand_angle() (randf() * M_PI * 2.0)

#endif
