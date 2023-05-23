#include "user/ustack.h"
#include "user/user.h"

void main()
{
    printf("ustack, world!\n");
    int *a = (int *)ustack_malloc(200);
    printf("a = %d\n", *a);
    *a = 5;
    printf("a = %d\n", *a);
    int *b = (int *)ustack_malloc(512);
    printf("b = %d\n", *b);
    *b = 6;
    printf("b = %d\n", *b);

    int freed_memory = ustack_free();
    printf("freed memory = %d\n", freed_memory);
    printf("b = %d\n", *b);

    int *c = (int *)ustack_malloc(1000);
    printf("c = %d\n", *c);
    *c = 7;
    printf("c = %d\n", *c);
    printf("b = %d\n", *b);
    freed_memory = ustack_free();
    printf("freed memory = %d\n", freed_memory);
    freed_memory = ustack_free();
    printf("freed memory = %d\n", freed_memory);
    freed_memory = ustack_free();
    printf("freed memory = %d\n", freed_memory);


    // printf("malloc, world!\n");
    // int *_a = (int *)malloc(4);
    // printf("a = %d\n", *_a);
    // *_a = 5;
    // printf("a = %d\n", *_a);
    // int *_b = (int *)malloc(4);
    // printf("b = %d\n", *_b);
    // *_b = 6;
    // printf("b = %d\n", *_b);

    // free(b);
    // printf("b = %d\n", *_b);

    // int *_c = (int *)malloc(4);
    // printf("c = %d\n", *_c);
    // *_c = 7;
    // printf("c = %d\n", *_c);
    // printf("b = %d\n", *_b);

    printf("done\n");
}