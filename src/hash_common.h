#ifndef GHH_HASH_COMMON_H
#define GHH_HASH_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// 64 bit
#if INTPTR_MAX == INT64_MAX

typedef uint64_t hash_t;
#define FNV_PRIME 0x00000100000001b3
#define FNV_BASIS 0xcbf29ce484222325

#else

// 32 bit
typedef uint32_t hash_t;
#define FNV_PRIME 0x01000193a
#define FNV_BASIS 0x811c9dc5

#endif

// fnv-1a hash function (http://isthe.com/chongo/tech/comp/fnv/)
static inline hash_t hash_key(int key_size, const void *key) {
	hash_t hash = FNV_BASIS;
	const uint8_t *bytes = key;

	if (key_size < 0) {
		while (*bytes) {
			hash ^= *bytes++;
			hash *= FNV_PRIME;
		}
	} else {
		for (int i = 0; i < key_size; ++i) {
			hash ^= bytes[i];
			hash *= FNV_PRIME;
		}
	}

	return hash;
}


static inline const void *copy_key(int key_size, const void *key) {
	void *copied;

	if (key_size < 0) {
		size_t num_bytes = 1;
		char *str = (char *)key;

		while (*str++)
			++num_bytes;

		num_bytes *= sizeof(*str);

		copied = malloc(num_bytes);
		memcpy(copied, key, num_bytes);
	} else {
		copied = malloc(key_size);
		memcpy(copied, key, key_size);
	}

	return copied;
}


static inline bool key_equals(int key_size, const void *key, const void *other) {
	size_t i = 0;
	const uint8_t *kbytes = key, *obytes = other;

	if (key_size < 0) {
		do {
			if (kbytes[i] != obytes[i])
				return false;
		} while (kbytes[i++]);
	} else {
		for (; i < key_size; ++i)
			if (kbytes[i] != obytes[i])
				return false;
	}

	return true;
}

#endif
