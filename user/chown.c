#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 4){
    fprintf(2, "Usage: chown <path> <uid> <gid>\n");
    exit(1);
  }

  int uid = atoi(argv[2]);
  int gid = atoi(argv[3]);

  if(chown(argv[1], uid, gid) < 0){
    fprintf(2, "chown %s failed\n", argv[1]);
    exit(1);
  }

  exit(0);
}
