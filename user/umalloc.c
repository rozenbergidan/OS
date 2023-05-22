#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

/// @brief allocate memory of size nbytes
/// @param addressPointer 
void
free(void *addressPointer)
{
  Header *basePointer, *currPointer;

  basePointer = (Header*)addressPointer - 1;
  for(currPointer = freep; !(basePointer > currPointer && basePointer < currPointer->s.ptr); currPointer = currPointer->s.ptr)
    if(currPointer >= currPointer->s.ptr && (basePointer > currPointer || basePointer < currPointer->s.ptr))
      break;
  if(basePointer + basePointer->s.size == currPointer->s.ptr){ // if the blocks are adjuscent - merging their size and ptr
    basePointer->s.size += currPointer->s.ptr->s.size;
    basePointer->s.ptr = currPointer->s.ptr->s.ptr;
  } else // if blocks are not ajuscent - just changing the pointer to follow the next block
    basePointer->s.ptr = currPointer->s.ptr;
  if(currPointer + currPointer->s.size == basePointer){  // if the base block is just before the current header - merging their size and ptr
    currPointer->s.size += basePointer->s.size;
    currPointer->s.ptr = basePointer->s.ptr;
  } else
    currPointer->s.ptr = basePointer;
  freep = currPointer;
}

/// @brief expend the process memory by nu headers units
/// @param nu - number of header units
/// @return the pointer to the start of the free blocks
static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096) // if the number of units is less than 4096 - get 4096 units
    nu = 4096;
  p = sbrk(nu * sizeof(Header)); // expending the memory of the process
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p; 
  hp->s.size = nu; // changing the size of the header - to be the number of headers located
  free((void*)(hp + 1)); 
  return freep;
}

/// @brief allocate nbytes bytes of memory
/// @param nbytes 
/// @return 
void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1; // calculating the number of header units needed
  if((prevp = freep) == 0){ // if the free list is empty - initialize it
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){ // if the block is big enough
      if(p->s.size == nunits) // if the block is exactly the size needed - just changing the pointer to follow the next block
        prevp->s.ptr = p->s.ptr;
      else { // if the block is bigger than needed - splitting the block
        p->s.size -= nunits; // changing the size of the block
        p += p->s.size; // changing the pointer to the start of the free block
        p->s.size = nunits; // changing the size of the free block
      }
      freep = prevp; // changing the free list pointer
      return (void*)(p + 1);
    }
    if(p == freep) // if we reached the end of the free list - getting more memory
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
