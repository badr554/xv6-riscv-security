#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: chmod <path> <mode_octal>\n");
    exit(1);
  }

  // simple octal parser
  int mode = 0;
  char *s = argv[2];
  while(*s){
    if(*s < '0' || *s > '7'){
      fprintf(2, "chmod: invalid mode %s\n", argv[2]);
      exit(1);
    }
    mode = (mode << 3) | (*s - '0');
    s++;
  }

  if(chmod(argv[1], mode) < 0){
    fprintf(2, "chmod %s failed\n", argv[1]);
    exit(1);
  }

  exit(0);
}
