#ifndef GHH_UTILS_H
#define GHH_UTILS_H

#include <stdio.h>
#include <stdlib.h>

// common macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) (MAX(MIN(x, b), a))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// bitfield ops
#define BIT_GET(bitfield, index) ((bitfield >> (index)) & 1)
#define BIT_SET(bitfield, index) do {bitfield |= (0x1 << (index));} while (0)
#define BIT_CLR(bitfield, index) do {bitfield &= ~(0x1 << (index));} while (0)

// swap variables
#define SWAP(a, b, temp) do { temp = a; a = b; b = temp; } while (0)
#define SWAP_XOR(a, b) (((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b)))

// runtime error handling
typedef enum ghh_error { OK, ERR } error_e; // to be used as a return type

#define TRY(thing) do { if (thing) return ERR; } while (0)

// panic/debugging
#define PANIC(...)\
	do {\
		fprintf(stderr, "PANIC at %s:%d\n", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
		exit(-1);\
	} while (0)

#ifndef NDEBUG
#define ENSURE(cond, ...) if (!(cond)) PANIC(__VA_ARGS__)
#else
#define ENSURE(...)
#endif

#define UNREACHABLE PANIC("reached supposedly unreachable location.\n")

// common IO operations
char *file_read(const char *filepath, size_t *out_len);
void file_write(const char *filepath, char *text);

#endif
