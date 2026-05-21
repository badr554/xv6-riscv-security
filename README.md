# xv6 Medical Device Security Extension
This repository contains an extended version of the xv6-riscv operating system, modified to implement three integrated kernel-level security layers: role-based user authentication, UNIX-style file permissions, and a system call audit log. These features simulate the security architecture required by FDA and IEC 62443 standards for life-critical embedded medical devices.

Developed as part of the **12th Project** for the CCY4304 course.

---

## Course Information
* **University:** Arab Academy for Science, Technology and Maritime Transport
* **College:** College of Computing and Information Technology
* **Course Code:** CCY4304
* **Course Title:** Operating Systems Security
* **Lecturer:** Prof. Dr. Ayman Adel
* **Teaching Assistants (TAs):** Abdelrahman Solyman

---

## Implemented Security Features

### 1. User Authentication (Phase 1)
* **Process Credentials:** Extended `struct proc` with `uid`, `gid`, and `username`. Credentials are automatically inherited by child processes during `fork()`.
* **Password Hashing:** Integrated a self-contained kernel-space **SHA-256** implementation to hash credentials, avoiding storing plaintext passwords.
* **Credentials Database:** A preloaded credentials list in the kernel storing passwords for `admin` (UID 0), `patient` (UID 1), and `doctor` (UID 2).
* **Login Shell:** The system `init` process is configured to spawn the `login` prompt first. Access to the shell (`sh`) is blocked without valid credentials.
* **Syscalls:** Added `login()`, `whoami()`, `useradd()`, `userdel()`, and `passwd()`.

### 2. File Access Control (Phase 2)
* **Inode Permissions:** Extended `struct dinode` (on-disk) and `struct inode` (in-memory) to hold permission `mode` bits, `uid`, and `gid`.
* **Access Checks:** The kernel checks file permissions during `sys_open()`, `fileread()`, `filewrite()`, and `sys_exec()`. Admin (`uid=0`) bypasses all restrictions.
* **File Setup:** The filesystem creator `mkfs.c` pre-allocates directories and files with medical-role permissions:
  * `/patient/records` (uid=1, read-only `0444`)
  * `/dosage/insulin.log` (uid=2, write `0644`)
  * `/device/config` (uid=0, admin only `0600`)
  * `/audit/syscall.log` (uid=0, admin only `0400`)
* **Syscalls:** Added `chmod()` and `chown()`.

### 3. Syscall Audit Log (Phase 3)
* **Kernel Ring Buffer:** A thread-safe audit log ring buffer stores trap and system call records (PID, UID, syscall number, tick).
* **Trap Pretty Printing:** Traps/exceptions print descriptive names along with process details (PID, UID, EPC, STVAL) on the console.
* **Log Access:** The `audit_read()` system call allows only `uid=0` (admin) to read the log entries.
* **Intrusion Detection:** The audit log automatically records security violations and login events.

---

## How to Build and Run

### Prerequisites
You need the RISC-V GNU Toolchain and QEMU (version >= 7.2) installed on your system.

### Compiling and Starting xv6
```bash
make clean
make qemu
```

### Running the Compliance Test Suite
Once booted into the xv6 shell, run the automated verification program:
```bash
compliance_test
```
This program validates all 23 test cases (covering authentication, file access controls, and audit trails), printing a detailed compliance report and returning a standard exit code (`0` for success).
