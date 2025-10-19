#ifndef ZLIB_UTILS_H
#define ZLIB_UTILS_H

#include <stddef.h>

unsigned char* zlib_compress(const unsigned char* data, size_t size, size_t* out_size);
unsigned char* zlib_uncompress(const unsigned char* data, size_t comp_size, size_t* out_size);

#endif