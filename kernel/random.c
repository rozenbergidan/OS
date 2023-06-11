
#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define RANDOM_DEV 2
uint8 seed = 0x2A;

uint8 lfsr_char(uint8 lfsr)
{
    uint8 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
    lfsr = (lfsr >> 1) | (bit << 7);
    return lfsr;
}

// Read from /random device
int randomRead(int fd, void *dst, int n)
{
    if (dst == 0 || n < 0)
        return -1;

    char buffer[n];
    int numCopied = 0;
    for (int i = 0; i < n; i++)
    {
        seed = lfsr_char(seed);
        buffer[i] = seed;
    }

    struct proc *p = myproc();
    for (int i = 0; i < n; i++)
    {
        if (copyout(p->pagetable, (uint64)dst + i, &buffer[i], 1) == -1)
            break;

        numCopied++;
    }

    return numCopied;
}

int randomWrite(int fd, const void *src, int n)
{

    // printf("seed: %d\n", seed);
    if (n != 1)
        return -1; // Invalid number of bytes for seeding

    if (either_copyin(&seed, 1, (uint64)src, 1) == -1)
        return -1;
    // printf("seed: %d\n", seed);
    return 1; // Return 1 on success
}

void randominit(void)
{
    devsw[RANDOM_DEV].write = (int (*)(int, uint64, int))randomWrite;
    devsw[RANDOM_DEV].read = (int (*)(int, uint64, int))randomRead;
}