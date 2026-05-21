#include "kernel/types.h"
#include "user/user.h"

void secret_win(void) {
    printf("Success! You hijacked the control flow - 221001606\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    char buffer[16];
    char c;
    int i = 0;

    printf("Enter input: ");

    while(read(0, &c, 1) > 0 && c != '\n') {
        buffer[i] = c;
        i++;
    }

    buffer[i < 16 ? i : 15] = '\0';
    printf("You entered: %s\n", buffer);
    printf("Returning normally...\n");
    return 0;
}
