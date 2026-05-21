#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 1) {
    printf("Usage: whoami\n");
    exit(1);
  }

  int uid = whoami();
  printf("uid=%d\n", uid);
  exit(0);
}
