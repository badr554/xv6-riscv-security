#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#define AUDIT_BUF_SIZE 2048
struct audit_entry {
int pid;
int uid;
int syscall_no;
uint tick;
};
static struct audit_entry audit_buf[AUDIT_BUF_SIZE];
static int audit_head = 0;
static int audit_count = 0;
static struct spinlock audit_lock;
void
audit_init(void)
{
initlock(&audit_lock, "audit");
}
void
audit_log(int pid, int uid, int syscall_no, uint tick)
{
acquire(&audit_lock);
audit_buf[audit_head].pid = pid;
audit_buf[audit_head].uid = uid;
audit_buf[audit_head].syscall_no = syscall_no;
audit_buf[audit_head].tick = tick;
audit_head = (audit_head + 1) % AUDIT_BUF_SIZE;
if(audit_count < AUDIT_BUF_SIZE) audit_count++;
release(&audit_lock);
}
// Only uid=0 may call this; copies entries to user space
int
audit_read_entries(uint64 addr, int max, struct proc *p)
{
if(p->uid != 0) return -1; // EPERM
acquire(&audit_lock);
int n = (audit_count < max) ? audit_count : max;
int start_idx = (audit_count < AUDIT_BUF_SIZE) ? (audit_count - n) : ((audit_head - n + AUDIT_BUF_SIZE) % AUDIT_BUF_SIZE);
for(int i = 0; i < n; i++) {
  int idx = (start_idx + i) % AUDIT_BUF_SIZE;
  if(copyout(p->pagetable, addr + i * sizeof(struct audit_entry), (char*)&audit_buf[idx], sizeof(struct audit_entry)) < 0) {
    release(&audit_lock);
    return -1;
  }
}
release(&audit_lock);
return n;
}

