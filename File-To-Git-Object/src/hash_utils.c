#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash_utils.h"

void compute_file_hash(const unsigned char* data, size_t length, unsigned char* hash, unsigned int* hash_length)
{
     EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if(!ctx)
    {
        perror("EVP_MD_CTX_new failed");
        exit(EXIT_SUCCESS);
    }

    //Initialize SHA digest
    if(EVP_DigestInit_ex(ctx, EVP_sha1(), NULL) !=1 )
    {
        perror("EVP_DigestInit_ex failed");
        exit(EXIT_FAILURE);
    }

   if(EVP_DigestUpdate(ctx, data, length) != 1)
    {
          perror("EVP_DigestUpdate failed");
           EVP_MD_CTX_free(ctx);
           exit(EXIT_FAILURE);
    }

    if(EVP_DigestFinal_ex(ctx, hash, hash_length) !=1 )
    {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    EVP_MD_CTX_free(ctx);
}