#include "kernel/param.h"
#include "kernel/types.h"
#include "user.h"


void main(){

    int tid = kthread_id();
    printf("tid: %d\n", tid);
}