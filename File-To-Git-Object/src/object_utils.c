#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "zlib_utils.h"
#include "object_utils.h"
#include "hash_utils.h"
#include "file_utils.h"

void ensure_dir(const char* path)
{
    struct stat st = {0};
    if(stat(path, &st) == -1)
        if(mkdir(path, 0755) == -1)
        {
            perror("mkdir failed");
            exit(EXIT_FAILURE);
        }
}

static void get_object_path(const char* hash_str, char *out_path, size_t size)
{
    snprintf(out_path, size, ".mygit/objects/%.2s/%s", hash_str, hash_str + 2);
}

void create_object(const char* input_path)
{
    size_t file_size;
    unsigned char* file_data = read_file(input_path, &file_size);

    //Build a Git like object content: "blob <size>\0<data>"
    char header[64];
    int header_len = snprintf(header, sizeof(header), "blob %zu", file_size);
    size_t full_size = header_len + 1 + file_size;
    unsigned char* full_data = malloc(full_size);
    memcpy(full_data, header, header_len);
    full_data[header_len] = '\0';
    memcpy(full_data + header_len + 1, file_data, file_size);
    free(file_data);

    //Compute has over uncompressed object data
    unsigned char hash[64];
    unsigned int hash_length = 0;
    compute_file_hash(full_data, full_size, hash, &hash_length);

    //Convert to hex for readability
    char hash_string[hash_length*2 +1];
    for(unsigned int i =0; i<hash_length; ++i)
    {
        sprintf(&hash_string[i*2], "%02x", hash[i]);
    }
    hash_string[hash_length*2] = '\0';

    //Compress it before writing
    size_t comp_size;
    unsigned char* comp_data = zlib_compress(full_data, full_size, &comp_size);
    free(full_data);

    //Create directories
    ensure_dir(".mygit");
    ensure_dir(".mygit/objects");
    char subdir[64];
    snprintf(subdir, sizeof(subdir), ".mygit/objects/%.2s", hash_string);
    ensure_dir(subdir);

    char object_path[256];
    snprintf(object_path, sizeof(object_path), "%s/%s", subdir, hash_string + 2 );

    FILE *object_file = fopen(object_path, "wb");
    if(!object_file)
    {
        perror("Error creating output file");
        exit(EXIT_FAILURE);
    }

    fwrite(comp_data, 1, comp_size, object_file);
    fclose(object_file);
    free(comp_data);

    printf("sha1(%s) = %s\n",input_path, hash_string);
    printf("Stored at: %s\n", object_path);
}

void cat_file(const char *hash_string)
{
    char object_path[256];
    get_object_path(hash_string, object_path, sizeof(object_path));

    size_t comp_size;
    unsigned char* comp_data = read_file(object_path, &comp_size);
    if(!comp_data)
    {
        fprintf(stderr, "Error: Could not read object file %s\n", object_path);
        return;
    }

    size_t decomp_size;
    unsigned char* decomp_data = zlib_uncompress(comp_data, comp_size, &decomp_size);
    free(comp_data);

    if(!decomp_data)
    {
        fprintf(stderr, "Error: Failed to decompress oject file\n");
        return;
    }

    char* null_pos = memchr(decomp_data, '\0', decomp_size);
    if(!null_pos)
    {
        fprintf(stderr, "Error: Invalid object format\n");
        free(decomp_data);
        return;
    }

    printf("------ File Content ------\n");
    fwrite(null_pos+1, 1, decomp_size - (null_pos + 1 - (char*)decomp_data), stdout);
    printf("\n");

    free(decomp_data);
}