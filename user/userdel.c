#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: userdel <username>\n");
    exit(1);
  }

  if (userdel(argv[1]) < 0) {
    printf("userdel failed\n");
    exit(1);
  }

  printf("user %s deleted successfully\n", argv[1]);
  exit(0);
}
