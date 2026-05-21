#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: useradd <username> <password>\n");
    exit(1);
  }

  if (useradd(argv[1], argv[2]) < 0) {
    printf("useradd failed\n");
    exit(1);
  }

  printf("user %s added successfully\n", argv[1]);
  exit(0);
}
