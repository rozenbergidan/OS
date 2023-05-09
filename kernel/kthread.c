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
    initlock(&kt->lock, "kthread");
    kt->state = UNUSED;
    kt->parent = p;

    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}

struct kthread *mykthread()
{
  return mycpu()->kthread;
}

int alloctid(struct proc *p)
{
  int i;
  acquire(&p->tid_lock);
  i = p->tid_counter++;
  release(&p->tid_lock);
  return i;
}

struct kthread *allockthread(struct proc *p)
{
  struct kthread *kt;
  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->lock); // caueses panic: sched locks
    if (kt->state == UNUSED)
    {
      kt->tid = alloctid(p);
      kt->state = USED;
      kt->trapframe = get_kthread_trapframe(p, kt);
      memset(&kt->context, 0, sizeof(struct context));
      kt->context.ra = (uint64)forkret;
      kt->context.sp = kt->kstack + PGSIZE;
      return kt;
    }
    release(&kt->lock);
  }
  return 0;
}

void hellofunc(void)
{
  printf("Hello from a new thread!\n");
  kthread_exit(0);
}
int allockthread_create(struct proc *p, void (*start_func)(), void *stack, uint stack_size)
{
  struct kthread *kt;
  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->lock); // caueses panic: sched locks
    if (kt->state == UNUSED)
    {
      kt->tid = alloctid(p);
      kt->state = RUNNABLE;
      // kt->kstack = (uint64)stack;
      kt->trapframe = get_kthread_trapframe(p, kt);
      memset(&kt->context, 0, sizeof(struct context));
      kt->context.ra = (uint64)forkret;
      kt->context.sp = (uint64)stack + stack_size;
      kt->trapframe->epc = (uint64)start_func;
      printf("allockthread_create: kt->trapframe->epc = %p\n", start_func);
      int tid = kt->tid;
      release(&kt->lock);
      return tid;
    }
    release(&kt->lock);
  }
  return -1;
}

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

void freekthread(struct kthread *kt)
{

  kt->state = UNUSED;
  kt->chan = 0;
  kt->trapframe = 0;
  kt->killed = 0;
  memset(&kt->context, 0, sizeof(struct context));
  kt->xstate = 0;
  kt->tid = 0;
  // kt->kstack = 0;
}

// TODO: delte this after you are done with task 2.2
void allocproc_help_function(struct proc *p)
{
  p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

  p->kthread->context.sp = p->kthread->kstack + PGSIZE;
}