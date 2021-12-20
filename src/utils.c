#include <stdlib.h>

#include "utils.h"

static void file_check(FILE *fp, const char *filepath) {
    if (!fp)
        ERROR("could not find file \"%s\".\n", filepath);
}

char *file_read(const char *filepath, size_t *out_len) {
    FILE *fp = fopen(filepath, "r");

    file_check(fp, filepath);
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

    file_check(fp, filepath);
    fprintf(fp, text);

    fclose(fp);
}
