#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void test(char *name, int result, int expect_ok)
{
int ok = (expect_ok ? result >= 0 : result < 0);
printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
}

int
main(void)
{
printf("=== Permission Tests ===\n");

// Login as patient
login("patient", "patient123");
printf("\n-- As PATIENT (uid=1) --\n");
test("patient reads /patient/records",
open("/patient/records", O_RDONLY), 1);
test("patient BLOCKED from /device/config",
open("/device/config", O_RDONLY), 0);
test("patient BLOCKED from /audit/syscall.log",
open("/audit/syscall.log", O_RDONLY), 0);

// Login as doctor
login("doctor", "doctor123");
printf("\n-- As DOCTOR (uid=2) --\n");
test("doctor writes /dosage/insulin.log",
open("/dosage/insulin.log", O_WRONLY), 1);
test("doctor reads /patient/records",
open("/patient/records", O_RDONLY), 1);

// Login as admin
login("admin", "password");
printf("\n-- As ADMIN (uid=0) --\n");
test("admin reads /device/config",
open("/device/config", O_RDONLY), 1);
test("admin reads /audit/syscall.log",
open("/audit/syscall.log", O_RDONLY), 1);

exit(0);
}
