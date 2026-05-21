#include "kernel/types.h"
#include "user/user.h"

int main() {
    int local_var = 42;
    printf("Stack address: %p\n", &local_var);
    exit(0);
}
