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
static UstackHeader *baseptr_free;
static UstackHeader *freep;

static UstackHeader *
morecore(uint nu)
{
    char *p;
    UstackHeader *hp;

    if (nu < 4096)
        nu = 4096;

    p = sbrk(nu * sizeof(UstackHeader));
    printf(" returned from sbrk\n");
    printf("morecore: p = %d\n", p);
    if (p == (char *)-1)
        return 0;
    hp = (UstackHeader *)p;
    hp->s.size = nu;
    printf("freep = %d\n", freep);
    printf("hp->s.ptr = %p\n", hp->s.ptr);
    baseptr_free = hp + 1;
    ustack_free();
    printf("freep = %d\n", freep);
    return freep;
}

void *ustack_malloc(uint len)
{
    UstackHeader *p, *prevp;
    uint nunits;

    if (len > 512)
    {
        return (void *)-1;
    }

    nunits = (len + sizeof(UstackHeader) - 1) / sizeof(UstackHeader) + 1;
    if ((prevp = freep) == 0)
    {
        printf("init\n");
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr)
    {
        printf("start of loop p: %p\n", p);
        if (p->s.size >= nunits)
        { // big enough
            if (p->s.size == nunits)
            { // exactly
                printf("exactly\n");
                prevp->s.ptr = p->s.ptr;
            }
            else
            { // allocate tail end
                printf("allocate tail end\n");
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            printf("setting freep to prevp+1: %p\n", prevp);
            freep = prevp;
            printf("returning p+1: %p\n", p + 1);
            return (void *)(p + 1);
        }
        if (p == freep)
        {
            printf("calling morecore\n");
            if ((p = morecore(nunits)) == 0)
            {
                return (void *)-1;
            }
            printf("returned from morecore: %p\n", p);
        }
    }
}

void ustack_free(void)
{

    UstackHeader *basePointer;
    UstackHeader *currPointer;

    // Find the last allocated block
    basePointer = baseptr_free - 1;
    for (currPointer = freep; !(basePointer > currPointer && basePointer < currPointer->s.ptr); currPointer = currPointer->s.ptr)
        if (currPointer >= currPointer->s.ptr && (basePointer > currPointer || basePointer < currPointer->s.ptr))
            break;

    if (basePointer + basePointer->s.size == currPointer->s.ptr)
    { // if the blocks are adjuscent - merging their size and ptr
        basePointer->s.size += currPointer->s.ptr->s.size;
        basePointer->s.ptr = currPointer->s.ptr->s.ptr;
    }
    else // if blocks are not ajuscent - just changing the pointer to follow the next block
        basePointer->s.ptr = currPointer->s.ptr;
    if (currPointer + currPointer->s.size == basePointer)
    { // if the base block is just before the current header - merging their size and ptr
        currPointer->s.size += basePointer->s.size;
        currPointer->s.ptr = basePointer->s.ptr;
    }
    else
        currPointer->s.ptr = basePointer;
    freep = currPointer;
}

void print_ustack(void)
{
    UstackHeader *p = &base;
    for(;p != 0;p=p->s.ptr)
    {
        printf("p = %p\n", p);
        printf("p->s.ptr = %p\n", p->s.ptr);
        printf("p->s.size = %d\n", p->s.size);
    }
}