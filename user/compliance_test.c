#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static int passed = 0, total = 0;

struct audit_entry {
int pid; int uid; int syscall_no; uint tick;
};

void run(char *name, int result, int expect_ok)
{
total++;
int ok = expect_ok ? (result >= 0) : (result < 0);
if(ok) passed++;
printf(" [%s] T%d: %s\n", ok?"PASS":"FAIL", total, name);
}

int
main(void)
{
int fd;
printf("=== Medical Device Compliance Test Suite ===\n\n");

// --- PHASE 1: AUTHENTICATION ---
printf("-- Phase 1: Authentication --\n");
run("whoami returns non-negative", whoami(), 1);
run("bad password rejected", login("admin","wrongpass"), 0);
run("bad username rejected", login("nobody","anything"), 0);
run("patient login succeeds", login("patient","patient123"), 1);
run("patient uid=1", whoami() == 1 ? 0 : -1, 1);
run("doctor login succeeds", login("doctor","doctor123"), 1);
run("doctor uid=2", whoami() == 2 ? 0 : -1, 1);
run("admin login succeeds", login("admin","password"), 1);
run("admin uid=0", whoami() == 0 ? 0 : -1, 1);

// --- PHASE 2: FILE PERMISSIONS ---
printf("\n-- Phase 2: File Permissions --\n");
login("patient","patient123");
fd = open("/patient/records", O_RDONLY);
run("patient reads own records", fd, 1);
if(fd>=0) close(fd);
fd = open("/device/config", O_RDONLY);
run("patient BLOCKED /device/config", fd, 0);
if(fd>=0) close(fd);
fd = open("/audit/syscall.log", O_RDONLY);
run("patient BLOCKED /audit/syscall.log", fd, 0);
if(fd>=0) close(fd);

login("doctor","doctor123");
fd = open("/dosage/insulin.log", O_WRONLY);
run("doctor writes insulin log", fd, 1);
if(fd>=0){ write(fd, "dose:5u\n", 8); close(fd); }
fd = open("/patient/records", O_RDONLY);
run("doctor reads patient records", fd, 1);
if(fd>=0) close(fd);

login("admin","password");
fd = open("/device/config", O_RDONLY);
run("admin bypasses /device/config", fd, 1);
if(fd>=0) close(fd);
fd = open("/audit/syscall.log", O_RDONLY);
run("admin bypasses /audit/syscall.log", fd, 1);
if(fd>=0) close(fd);

run("chmod succeeds as admin", chmod("/patient/records", 0644), 1);
run("chown succeeds as admin", chown("/patient/records", 1, 1), 1);

// --- PHASE 3: AUDIT LOG ---
printf("\n-- Phase 3: Audit Log --\n");
login("patient","patient123");
run("non-admin audit_read returns -1", audit_read(0,0), 0);

login("admin","password");
static struct audit_entry buf[2048];
int n = audit_read((uint64)buf, 2048);
run("admin reads audit log", n, 1);
run("audit log has entries", n > 0 ? 0 : -1, 1);

// Check failed login was logged (syscall 22 = SYS_login)
int found = 0;
for(int i=0;i<n;i++) if(buf[i].syscall_no==22) { found=1; break; }
run("login attempts appear in audit log", found ? 0 : -1, 1);

printf("\n[AUDIT LOG EVIDENCE]\n");
int found_attack = 0;
for(int i=0;i<n;i++) {
    // Print logs as evidence
    printf("  Tick: %d | PID: %d | UID: %d | Syscall: %d\n", buf[i].tick, buf[i].pid, buf[i].uid, buf[i].syscall_no);
    // Detect patient (uid=1) attacking (syscall 15 = SYS_open)
    if (buf[i].uid == 1 && buf[i].syscall_no == 15) {
        found_attack = 1;
    }
}
run("attack by patient detected in log", found_attack ? 0 : -1, 1);

// --- SUMMARY ---
printf("\n=== Compliance Summary ===\n");
printf("Tests passed: %d / %d\n", passed, total);
if(passed == total)
printf("STATUS: COMPLIANT (FDA/IEC 62443 simulation)\n");
else
printf("STATUS: NON-COMPLIANT -- %d failure(s)\n", total-passed);
exit(passed == total ? 0 : 1);
}
