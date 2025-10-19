#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define BUFFER_SIZE 32768 //32 KB buffer

void compute_sha1_evp(const char * input_path)
{
    unsigned char buffer[BUFFER_SIZE];
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

    //Initialize SHA-1 digest
    if(EVP_DigestInit_ex(ctx, EVP_sha1(), NULL) !=1 )
    {
        perror("EVP_DigestInit_ex failed");
        fclose(input);
        exit(EXIT_FAILURE);
    }

    size_t byte_read;
    while((byte_read = fread(buffer, 1, BUFFER_SIZE, input)) !=0)
    {
        if(EVP_DigestUpdate(ctx, buffer, byte_read) != 1)
        {
            perror("EVP_DigestUpdate failed");
            EVP_MD_CTX_free(ctx);
            fclose(input);
            exit(EXIT_FAILURE);
        }
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

    //Convert to hex
    char hash_string[hash_length*2 +1];
    for(unsigned int i =0; i<hash_length; ++i)
    {
        sprintf(&hash_string[i*2], "%02x", hash[i]);
    }

    printf("SHA-1(%s) = %s\n", input_path, hash_string);

    //Write to file
    char output_path[1024];
    snprintf(output_path, sizeof(output_path), "%s.sha1", input_path);
    FILE *output = fopen(output_path, "w");
    if(!output)
    {
        perror("Error creating output file");
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

    compute_sha1_evp(argv[1]);
    return EXIT_SUCCESS;
}