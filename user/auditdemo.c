#include "kernel/types.h"
#include "user/user.h"

struct audit_entry {
int pid;
int uid;
int syscall_no;
uint tick;
};

int
main(void)
{
struct audit_entry buf[64];
int n = audit_read((uint64)buf, 64);
if(n < 0){
printf("audit_read: permission denied (not admin)\n");
exit(1);
}

printf("=== Audit Log (%d entries) ===\n", n);
for(int i = 0; i < n; i++){
printf("[tick %d] PID=%d UID=%d SYSCALL=%d\n",
buf[i].tick, buf[i].pid, buf[i].uid, buf[i].syscall_no);
}
exit(0);
}
