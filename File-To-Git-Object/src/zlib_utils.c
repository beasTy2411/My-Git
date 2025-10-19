#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "zlib_utils.h"

//Compress data with zlib
unsigned char* zlib_compress(const unsigned char* data, size_t size, size_t *out_size )
{
    uLongf comp_bound = compressBound(size);
    unsigned char* comp_data = malloc(comp_bound);
    if(!comp_data)
    {
        perror("Malloc Failed");
        exit(EXIT_FAILURE);
    }

    if(compress2(comp_data, &comp_bound, data, size, Z_BEST_COMPRESSION) != Z_OK)
    {
        fprintf(stderr, "Compression Failed\n");
        exit(EXIT_FAILURE);
    }

    *out_size = comp_bound;
    return comp_data;
}

unsigned char* zlib_uncompress(const unsigned char* data, size_t comp_size, size_t* out_size)
{
    unsigned long bound = comp_size*10;
    unsigned char *out = malloc(bound);
    if(!out)
    {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }

    int ret = uncompress(out, &bound, data, comp_size);
    if(ret == Z_BUF_ERROR)
    {
        bound *= 4;
        out = realloc(out, bound);
        ret = uncompress(out, &bound, data,comp_size);
    }

    if(ret!= Z_OK)
    {
        fprintf(stderr, "Decompression Failed\n");
        exit(EXIT_FAILURE);
    }

    *out_size = bound;
    return out;
}