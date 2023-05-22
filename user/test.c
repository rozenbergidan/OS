#include "user/ustack.h"
#include "user/user.h"

void main()
{
    int *a = (int *)ustack_malloc(512);
    int *b = (int *)ustack_malloc(512);
    int *c = (int *)ustack_malloc(512);
    int *d = (int *)ustack_malloc(512);
    int *e = (int *)ustack_malloc(512);
    int *f = (int *)ustack_malloc(512);
    int *g = (int *)ustack_malloc(512);
    int *h = (int *)ustack_malloc(512);
    printf("a = %d\n", *a);
    printf("b = %d\n", *b);
    printf("c = %d\n", *c);
    printf("d = %d\n", *d);
    printf("e = %d\n", *e);
    printf("f = %d\n", *f);
    printf("g = %d\n", *g);
    printf("h = %d\n", *h);
    // printf("a = %d\n", *a);
    // *a = 5;
    // printf("a = %d\n", *a);
    // // ustack_free();
    // int *b = (int *)ustack_malloc(4);
    // printf("b = %d\n", *b);
    // *b = 6;
    // printf("b = %d\n", *b);
    // printf("a = %d\n", *a);

    print_ustack();
    // ustack_free();
    // printf("b = %d\n", *b);
    // printf("b = %d\n", *b);
    // int *c = (int *)ustack_malloc(4);
    // printf("b = %d\n", *b);    
    // printf("c = %d\n", *c);

    // int *c = (int*)ustack_malloc(4);
    // *c = 7;
    // ustack_free();
    // ustack_free();
    // ustack_free();
    printf("done\n");
    // print_ustack();
}