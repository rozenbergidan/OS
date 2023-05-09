#include "kernel/param.h"
#include "kernel/types.h"
#include "user.h"

void sayhello2(){
    printf("hello2\n");
    kthread_exit(0);
};

void sayhello()
{
    printf("hello\n");
    kthread_exit(0);
}

int main()
{
    printf("main\n");
    int status;
    void *stack = malloc(MAX_KSTACK_SIZE);
    printf("stack: %p\n", stack);
    printf("sayhello: %p\n", sayhello);
    int tid = kthread_create(&sayhello, stack, (uint)MAX_KSTACK_SIZE);
    printf("tid: %d\n", tid);
    kthread_join(tid, &status);
    printf("joined\n");
    exit(0);
}
