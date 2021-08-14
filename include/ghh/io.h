#ifndef GHH_IO_H
#define GHH_IO_H

// string must be freed
char *io_read_text(const char *filename);

void io_sprint(char *buffer, const char *format_string, );
void io_print();

#endif
