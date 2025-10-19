#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "object_utils.h"

#define BUFFER_SIZE 32768 //32 KB buffer


int main (int argc, char *argv[])
{
    if(argc < 3)
    {   
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, " %s hash-object <file>\n", argv[0]);
        fprintf(stderr, " %s cat-file -p <hash>\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "hash-object") == 0)
    {
        create_object(argv[2]);
    }
    else if(strcmp(argv[1], "cat-file") == 0 && strcmp(argv[2], "-p") == 0)
    {
        cat_file(argv[3]);
    }
    else
        fprintf(stderr, "Invalid command\n");
        
    return 0;
}