#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern void forkret(void);

// return the next ktid from the process
int allocktid(struct proc *p)
{
  int ktid;

  acquire(&p->thread_id_counter_lock);
  ktid = p->thread_id_counter;
  p->thread_id_counter++;
  release(&p->thread_id_counter_lock);

  return ktid;
}

struct kthread *allockthread(struct proc *p)
{
  struct kthread *kt;
  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->lock);
    if (kt->state == UNUSED)
    {
      goto found;
    }
    release(&kt->lock);
  }
  return 0;

found:
  kt->state = USED;
  kt->trapframe = get_kthread_trapframe(p, kt);
  memset(&kt->context, 0, sizeof(kt->context));
  kt->context.ra = (uint64)forkret;
  kt->context.sp = kt->kstack + PGSIZE;

  return kt;
}
// initialize the kthread group table of the process
void kthreadinit(struct proc *p)
{

  initlock(&p->thread_id_counter_lock, "thread_id_counter_lock");

  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->lock, "kthread_lock");
    kt->state = UNUSED;
    kt->parent = p;
    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}

// return the current running kthread from cpu
struct kthread *mykthread(void)
{
  push_off();
  struct cpu *c = mycpu();
  struct kthread *kt = c->kthread;
  pop_off();
  return kt;
}

// return the thread trapframe based on the process based trapframe
struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

// free the kthread
// kt lock must be held
void freekthread(struct kthread *kt)
{
  if(kt->trapframe)
    kfree((void*)kt->trapframe);
  kt->trapframe = 0;
  memset(&kt->context, 0, sizeof(kt->context));
  kt->state = UNUSED;
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->kstack = 0;
  kt->ktid = 0;
}

// TODO: delte this after you are done with task 2.2
// void allocproc_help_function(struct proc *p)
// {
//   p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

//   p->kthread->context.sp = p->kthread->kstack + PGSIZE;
// }