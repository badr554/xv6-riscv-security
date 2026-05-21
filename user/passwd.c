#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: passwd <username> <new_password>\n");
    exit(1);
  }

  if (passwd(argv[1], argv[2]) < 0) {
    printf("passwd failed\n");
    exit(1);
  }

  printf("password for %s changed successfully\n", argv[1]);
  exit(0);
}
