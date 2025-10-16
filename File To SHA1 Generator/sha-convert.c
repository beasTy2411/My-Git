#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define BUFFER_SIZE 32768

void compute_sha1(const char* input_path)
{
    unsigned char buffer[BUFFER_SIZE];
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha_ctx;
    FILE* input = fopen(input_path, "rb");
    if(!input)
    {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    SHA1_Init(&sha_ctx);

    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) !=0)
    {
        SHA1_Update(&sha_ctx, buffer, bytes_read);
    }

    SHA1_Final(hash, &sha_ctx);
    fclose(input);

    char hash_string[SHA_DIGEST_LENGTH*2 + 1];
    for(int i = 0; i<SHA_DIGEST_LENGTH; ++i)
        sprintf(&hash_string[i*2], "%02x", hash[i]);
    
    printf("SHA1(%s) = %s\n", input_path, hash_string);

    char output_path[1024];
    snprintf(output_path, sizeof(output_path), "%s.sha1", input_path);
    FILE *output = fopen(output_path, "w");
    if(!output)
    {
        perror("Error in creating the ouput file");
        exit(EXIT_FAILURE);
    }

    fprintf(output, "%s\n", hash_string);
    fclose(output);

    printf("Hash written to: %s\n", output_path);
}

int main (int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    compute_sha1(argv[1]);
    return EXIT_SUCCESS;
}