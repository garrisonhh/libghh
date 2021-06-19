#include <stdlib.h>
#include <string.h>
#include <ghh/hashable.h>

hash_t hash_unsigned(const void *u_num) {
	hash_t hash = *(unsigned *)u_num;

	for (int i = 0; i < 4; i++)
		hash = hash + (hash ^ (hash * 33));

	return hash;
}

int cmp_unsigned(const void *a, const void *b) {
	return *(unsigned *)a - *(unsigned *)b;
}

const void *copy_unsigned(const void *u) {
	unsigned *ucpy = malloc(sizeof(unsigned));
	*ucpy = *(unsigned *)u;

	return ucpy;
}

hash_t hash_string(const void *string) {
	hash_t hash = 0;
	char *str = (char *)string;

	while (*str++)
		hash = (hash << 2) | ((hash_t)*str);

	return hash;
}

int cmp_string(const void *a, const void *b) {
	return strcmp(a, b);
}

const void *copy_string(const void *string) {
	char *str = malloc(sizeof(char) * (strlen(string) + 1));

	strcpy(str, string);

	return str;
}

hashmap_funcs_t hashmap_funcs(hashable_e hashable) {
	switch (hashable) {
	case HASH_UNSIGNED:
		return (hashmap_funcs_t){
			.cmp = cmp_unsigned,
			.hash = hash_unsigned,
			.copy = copy_unsigned
			};
		case HASH_STRING:
			return (hashmap_funcs_t){
				.cmp = cmp_string,
				.hash = hash_string,
				.copy = copy_string
			};
		default:
			exit(1);
	}
}
