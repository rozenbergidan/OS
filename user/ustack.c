#include "ustack.h"
#include "user.h"

struct ustack_header
{
    uint len;
    struct ustack_header *prev;
};

// void _free(void *ap)
// {
//     UstackHeader *bp, *p;
//     bp = (UstackHeader *)ap - 1;
// }

static struct ustack_header base;
static struct ustack_header *last = 0;

static struct ustack_header *morecore(uint nunits)
{
    // int size = (nunits + sizeof(struct ustack_header) - 1) / sizeof(struct ustack_header) + 1;
    char *p = sbrk(nunits);
    if (p == (char *)-1)
    {
        return 0;
    }
    struct ustack_header *hp = (struct ustack_header *)p;
    hp->len = nunits;
    hp->prev = last;
    last = hp;
    return hp;
}

void *ustack_malloc(uint len)
{
    // last == 0; means that it is the first time we call malloc and we need to allocate the base
    if (last == 0)
    {
        last = &base;
        last->prev = 0;
        last->len = 0;
    }
    // check if len is bigger then 512
    if (len > 512)
    {
        return (void *)-1;
    }
    // calculate how meny units we need to allocate

    struct ustack_header *p;
    if (last->len + len > 4096)
    {
        uint nunits = (len + sizeof(struct ustack_header) - 1) / sizeof(struct ustack_header) + 1;
        len = nunits * sizeof(struct ustack_header);
        // allocate the memory
        p = morecore(len);
        if (p == 0)
        {
            return (void *)-1;
        }

        // update last to the new allocated memory
        last = p;
        // return the pointer to the start of the allocated memory
        return (void *)(p + 1);
    }
    else
    {
        p = last;
        last = last + len;
        last->prev = p;
        last->len = len;
        return (void *)(p + 1);
    }
    // }
}

int ustack_free(void)
{

    // free last and update last to be the previous header
    if (last == 0)
    {
        return -1;
    }
    // printf("len is more then 0. last->len = %d\n", last->len);
    struct ustack_header *p = last;
    int freed_space = p->len;
    last = last->prev;
    // printf("callsing sbrk with -%d\n", p->len);
    char *psbrk = sbrk(-(p->len));
    // printf("after sbrk = %d\n", freed_space);
    if (psbrk == (char *)-1 || freed_space == 0)
    {
        return -1;
    }
    return freed_space;
}

void print_ustack(void)
{
    struct ustack_header *p;
    for (p = last; p != 0; p = p->prev)
    {
        printf("p = %p\n", p);
        printf("p->len = %d\n", p->len);
        printf("p->prev = %p\n", p->prev);
    }
}