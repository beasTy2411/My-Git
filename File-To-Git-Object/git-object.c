#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <zlib.h>
#include <errno.h>

#define BUFFER_SIZE 32768 //32 KB buffer


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

/* Function hashes the file create another file containing the hash 
and prints the hash to the terminal */
void compute_sha1_evp(const char *algo_name, const char * input_path)
{   
    const EVP_MD* md = EVP_get_digestbyname(algo_name);
    if(!md)
    {
        fprintf(stderr, "Error: Unknown algorithm '%s'\n", algo_name);
        fprintf(stderr, "Supported: sha1, sha224, sha256, sha384, sha512\n");
        exit(EXIT_FAILURE);
    }

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
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    FILE *input = fopen(input_path, "rb");
    if(!input)
    {
        perror("Error in opening input file");
        exit(EXIT_FAILURE);
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if(!ctx)
    {
        perror("EVP_MD_CTX_new failed");
        fclose(input);
        exit(EXIT_SUCCESS);
    }

    //Initialize SHA digest
    if(EVP_DigestInit_ex(ctx, md, NULL) !=1 )
    {
        perror("EVP_DigestInit_ex failed");
        fclose(input);
        exit(EXIT_FAILURE);
    }

   if(EVP_DigestUpdate(ctx, full_data, full_size) != 1)
    {
          perror("EVP_DigestUpdate failed");
           EVP_MD_CTX_free(ctx);
           fclose(input);
           exit(EXIT_FAILURE);
    }

    if(EVP_DigestFinal_ex(ctx, hash, &hash_length) !=1 )
    {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(ctx);
        fclose(input);
        exit(EXIT_FAILURE);
    }

    EVP_MD_CTX_free(ctx);
    fclose(input);

    //Convert to hex for readability
    char hash_string[hash_length*2 +1];
    for(unsigned int i =0; i<hash_length; ++i)
    {
        sprintf(&hash_string[i*2], "%02x", hash[i]);
    }
    hash_string[hash_length*2] = '\0';

    printf("%s(%s) = %s\n", algo_name, input_path, hash_string);

    //Compress it before writing
    size_t comp_size;
    unsigned char* comp_data = zlib_compress(full_data, full_size, &comp_size);
    free(full_data);

    //Create the .mygit/objects directory
    ensure_dir(".mygit");
    ensure_dir(".mygit/objects");

    //subdir
    char subdir[64];
    snprintf(subdir, sizeof(subdir), ".mygit/objects/%.2s", hash_string);
    ensure_dir(subdir);

    //File path
    char object_path[256];
    snprintf(object_path, sizeof(object_path), "%s/%s", subdir, hash_string + 2);

    input = fopen(input_path, "rb");
    if(!input)
    {
        perror("Error in reopening file for reading");;
        exit(EXIT_FAILURE);
    }

    //Write to file
    FILE *object_file = fopen(object_path, "wb");
    if(!object_file)
    {
        perror("Error creating output file");
        exit(EXIT_FAILURE);
    }

    fwrite(comp_data, 1, comp_size, object_file);

    fclose(input);
    fclose(object_file);
    free(comp_data);

    printf("Object stored at: %s\n", object_path);
}

int main (int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <algorithm> <file>\n", argv[0]);
        fprintf(stderr, "Example: %s sha256 myfile.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    compute_sha1_evp(argv[1], argv[2]);
    return EXIT_SUCCESS;
}