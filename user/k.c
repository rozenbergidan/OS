#include "kernel/param.h"
#include "kernel/types.h"
#include "user.h"

void sayhello2(void){
    printf("hello2\n");
    kthread_exit(0);
    return;
};

void sayhello(void)
{
    printf("hello\n");
    kthread_exit(0);
    return;
}

int main()
{
    printf("main\n");
    // int status;
    uint64 stack = (uint64)malloc(MAX_KSTACK_SIZE);
    // printf("stack: %p\n", stack);
    // printf("sayhello: %p\n", (void *)sayhello);
    kthread_create((void *(*)())sayhello, (void*)stack, MAX_KSTACK_SIZE);
    // printf("tid: %d\n", tid);
    // kthread_join(tid, &status);
    // printf("joined\n");
    exit(0);
}
