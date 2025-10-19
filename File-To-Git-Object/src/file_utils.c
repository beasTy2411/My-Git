#include <stdio.h>
#include <stdlib.h>

#include "file_utils.h"

//Read entire file into memory
unsigned char* read_file(const char* path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if(!f)
    {
        perror("Error in opening the file");
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* data = malloc(size);
    if(!data)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    fread(data, 1, size, f);
    fclose(f);
    *out_size = size;
    return data;
}