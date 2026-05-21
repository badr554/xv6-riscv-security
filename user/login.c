#include "kernel/types.h"
#include "user/user.h"
int
main(void)
{
char username[16];
char password[32];
for(;;){
printf("\nxv6 Medical Device Login\n");
printf("username: ");
gets(username, sizeof(username));
// strip newline
for(int i = 0; username[i]; i++)
if(username[i] == '\n'){ username[i] = 0; break; }
printf("password: ");
gets(password, sizeof(password));
for(int i = 0; password[i]; i++)
if(password[i] == '\n'){ password[i] = 0; break; }
int uid = login(username, password);
if(uid >= 0){
char *role = (uid==0)?"ADMIN":(uid==1)?"PATIENT":"DOCTOR";
printf("Login successful. Role: %s (uid=%d)\n", role, uid);
char *argv[] = { "sh", 0 };
exec("sh", argv);
printf("exec sh failed\n");
} else {
printf("Login failed. Invalid credentials.\n");
}
}
}
