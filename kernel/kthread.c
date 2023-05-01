#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern void forkret(void);

void kthreadinit(struct proc *p)
{

  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->kthread_lock, "kthread_lock");
    kt->kthread_state = UNUSED;
    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}

struct kthread *mykthread()
{
  return &myproc()->kthread;
}

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

// TODO: delte this after you are done with task 2.2
// void allocproc_help_function(struct proc *p)
// {
//   p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

//   p->kthread->context.sp = p->kthread->kstack + PGSIZE;
// }

int alloc_kthread_id(struct proc *p)
{

  acquire(&p->kthread_counter_lock);
  int tid = p->kthread_counter++;
  release(&p->kthread_counter_lock);
  return tid;
}

static struct kthread *alloc_kthread(struct proc *p)
{
  struct kthread *kt;
  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->kthread_lock);
    if (kt->kthread_state == UNUSED)
    {
      kt->kthread_state = USED;
      kt->kthread_tid = alloc_kthread_id(p);
      kt->trapframe = get_kthread_trapframe(p, kt);
      memset(&kt->context, 0, sizeof(kt->context));
      kt->context.ra = (uint64)forkret;
      kt->context.sp = kt->kstack + PGSIZE;
      return kt;
    }
    release(&kt->kthread_lock);
  }
  return 0;
}

static void free_kthread(struct kthread *kt)
{
  if(kt->trapframe)
    kfree((void *)kt->trapframe);
  kt->trapframe = 0;
  kt->kthread_killed = 0;
  kt->kthread_xstate = 0;
  kt->kthread_tid = 0;
  kt->kthread_chan = 0;
  kt->kthread_parent = 0;
  kt->kthread_state = UNUSED;
}