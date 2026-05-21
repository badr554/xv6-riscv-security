#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "syscall.h"
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
// login(username, password) -> uid on success, -1 on failure
uint64
sys_login(void)
{
  char user[16], pass[32];
  argstr(0, user, sizeof(user));
  argstr(1, pass, sizeof(pass));
  int uid = checkpasswd(user, pass);
  struct proc *p = myproc();
  if(uid >= 0){
    p->uid = uid;
    p->gid = uid;
    safestrcpy(p->username, user, sizeof(p->username)); // Bug 3 fix: store username
    audit_log(p->pid, uid, SYS_login, ticks);
  } else {
    audit_log(p->pid, -1, SYS_login, ticks);
  }
  return uid;

}
// whoami() -> current uid
uint64
sys_whoami(void)
{
  return myproc()->uid;
}

// audit_read(buf, max) -> number of entries written, -1 if not admin
uint64
sys_audit_read(void)
{
  uint64 addr;
  int max;
  argaddr(0, &addr);
  argint(1, &max);
  return audit_read_entries(addr, max, myproc());
}

uint64
sys_useradd(void)
{
  char username[16];
  char password[32];
  argstr(0, username, sizeof(username));
  argstr(1, password, sizeof(password));
  if (myproc()->uid != 0) return -1;
  return useradd(username, password);
}

uint64
sys_userdel(void)
{
  char username[16];
  argstr(0, username, sizeof(username));
  if (myproc()->uid != 0) return -1;
  return userdel(username);
}

uint64
sys_passwd(void)
{
  char username[16];
  char password[32];
  argstr(0, username, sizeof(username));
  argstr(1, password, sizeof(password));
  int caller_uid = myproc()->uid;
  int target_uid = get_uid_by_username(username);
  if (caller_uid != 0 && caller_uid != target_uid) return -1;
  return passwd(username, password);
}
