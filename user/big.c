#include "kernel/types.h"
#include "user.h"

void main(int argc, char **argv)
{
    // void* ptr = malloc(100);
    // printf("ptr: %p\n", ptr);
    int howmany = 0;
    if (argc > 1)
    {
        howmany = atoi(argv[1]);
    }

    void *pointers[howmany];
    for (int i = 0; i < howmany; i++)
    {
        pointers[i] = malloc(100);
    }

    if (argc > 2)
    {
        howmany = atoi(argv[2]);
        if (howmany == 1)
        {
            for (int i = 0; i < 100; i++)
            {
                free(pointers[i]);
            }
        }
    }
}