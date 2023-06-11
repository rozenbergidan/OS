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

#define RANDOM_DEV 7

uint8 seed = 0x2A;

// Linear feedback shift register
// Returns the next pseudo-random number
// The seed is updated with the returned value
uint8 lfsr_char(uint8 lfsr)
{
    uint8 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
    lfsr = (lfsr >> 1) | (bit << 7);
    return lfsr;
}

int randomRead(int fd, uint64 dst, int n)
{
    printf("randomRead called\n");
    char *buffer = (char *)dst;

    for (int i = 0; i < n; i++)
    {
        buffer[i] = lfsr_char(seed);
    }

    return n;
}

int randomWrite(int fd, uint64 src, int n)
{
    if (n != 1)
        return -1; // Invalid number of bytes for seeding

    seed = *(uint8 *)src;
    return 1; // Return 1 on success
}

void randominit(void)
{
    devsw[RANDOM_DEV].write = randomWrite;
    devsw[RANDOM_DEV].read = randomRead;
}