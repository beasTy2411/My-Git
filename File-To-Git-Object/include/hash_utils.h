#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <stddef.h>

void compute_file_hash(const unsigned char* data, size_t length, unsigned char* hash, unsigned int* hash_len);

#endif