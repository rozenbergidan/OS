#include "ustack.h"
#include "user.h"

typedef union ustack_header
{
    struct
    {
        union ustack_header *ptr;
        uint size;
    } s;
    long x;
} UstackHeader;

static UstackHeader base;
static UstackHeader *freep;
static void *lastToFree;
static void *ap;
static int flagToFree = 0;

// void _free(void *ap)
// {
//     UstackHeader *bp, *p;
//     bp = (UstackHeader *)ap - 1;
// }

static UstackHeader *morecore(uint nunits)
{
    char *p;
    UstackHeader *hp;
    if (nunits < 4096)
        nunits = 4096;
    p = sbrk(nunits * sizeof(UstackHeader));
    if (p == (char *)-1)
        return 0;
    hp = (UstackHeader *)p;
    hp->s.size = nunits;
    ap = (void *)(hp + 1);
    flagToFree = 1;
    ustack_free();
    flagToFree = 0;
    return freep;
}

void *ustack_malloc(uint len)
{

    UstackHeader *currentPtr, *prevPtr;
    uint nunits;
    if (len > 512)
    {
        return (void *)-1;
    }

    nunits = (len + sizeof(UstackHeader) - 1) / sizeof(UstackHeader) + 1;
    if ((prevPtr = freep) == 0)
    {
        base.s.ptr = &base;
        freep = &base;
        prevPtr = &base;
        base.s.size = 0;
    }
    for (currentPtr = prevPtr->s.ptr;; prevPtr = currentPtr, currentPtr = currentPtr->s.ptr)
    {
        if (currentPtr == freep)
        {
            if ((currentPtr = morecore(nunits)) == 0)
                return (void *)-1;
        }
        else
        {
            if (currentPtr->s.size >= nunits)
            {
                if (currentPtr->s.size == nunits)
                    prevPtr->s.ptr = currentPtr->s.ptr;
                else
                {
                    currentPtr->s.size -= nunits;
                    currentPtr += currentPtr->s.size;
                    currentPtr->s.size = nunits;
                }
                freep = prevPtr;
                lastToFree = currentPtr;

                return (void *)(currentPtr + 1);
            }
        }
    }
}

int ustack_free(void)
{
    // char *p;
    UstackHeader *bufferPointer, *currPointer;
    int freed_size = 0;

    if (flagToFree == 1)
    { // internal free???
        bufferPointer = (UstackHeader *)ap - 1;
    }
    else
    {
        bufferPointer = (UstackHeader *)lastToFree - 1; // api free
    }

    if (freep == 0)
    { // trying to free empty list
        return -1;
    }
    freed_size = bufferPointer->s.size * sizeof(UstackHeader);
    for (currPointer = freep; !(bufferPointer > currPointer && bufferPointer < currPointer->s.ptr); currPointer = currPointer->s.ptr)
        if (currPointer >= currPointer->s.ptr && (bufferPointer > currPointer || bufferPointer < currPointer->s.ptr))
            break;

    if (bufferPointer + bufferPointer->s.size == currPointer->s.ptr)
    { // if the blocks are adjuscent - merging their size and ptr
        bufferPointer->s.size += currPointer->s.ptr->s.size;
        bufferPointer->s.ptr = currPointer->s.ptr->s.ptr;
    }
    else // if blocks are not ajuscent - just changing the pointer to follow the next block
        bufferPointer->s.ptr = currPointer->s.ptr;
    if (currPointer + currPointer->s.size == bufferPointer)
    { // if the base block is just before the current header - merging their size and ptr
        currPointer->s.size += bufferPointer->s.size;
        currPointer->s.ptr = bufferPointer->s.ptr;
    }
    else
        currPointer->s.ptr = bufferPointer;

    freep = currPointer;
    // if(freep == &base){
    // p = sbrk(-(freep->s.size * sizeof(UstackHeader)));
    // if (p == (char *)-1)
    //     return -1;
    // }
    return freed_size;
}

void print_ustack(void)
{
    UstackHeader *p = &base;
    for (; p != 0; p = p->s.ptr)
    {
        printf("p = %p\n", p);
        printf("p->s.ptr = %p\n", p->s.ptr);
        printf("p->s.size = %d\n", p->s.size);
    }
}