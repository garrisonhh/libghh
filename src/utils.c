#include <stdlib.h>

#include "utils.h"

char *file_read(const char *filepath, size_t *out_len) {
    FILE *fp = fopen(filepath, "r");

    fseek(fp, 0, SEEK_END);

    size_t len = ftell(fp);
    char *text = malloc(len + 1);

    text[len] = '\0';
    rewind(fp);
    fread(text, 1, len, fp);

    fclose(fp);

    if (out_len)
        *out_len = len;

    return text;
}


void file_write(const char *filepath, char *text) {
    FILE *fp = fopen(filepath, "w");

    fprintf(fp, text);

    fclose(fp);
}
