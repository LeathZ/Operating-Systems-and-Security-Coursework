#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    //Creating Users through System calls
    printf("Creating system user accounts...\n");
    system("useradd -m user1");
    system("useradd -m user2");
    system("useradd -m user3");

    //Assigning passwords to our new users
    system("echo 'user1:aekaiemna' | chpasswd");
    system("echo 'user2:aekaiemna' | chpasswd");
    system("echo 'user3:aekaiemna' | chpasswd");
    printf("Accounts created.\n");

    //Creating files and assigning permisssions
    printf("Making files and assigning permissions to the users...\n");

    char *file1 = "user1_readonly.txt";
    char *file2 = "user2_readwrite.txt";
    char *file3 = "user3_executeonly.txt";

    //Creating files with different permissions
    creat(file1, S_IRUSR);
    creat(file2, S_IRUSR | S_IWUSR);
    creat(file3, S_IXUSR);

    //Changing permission of files to the newly created users
    //Since after creation the file is only owned by root we change ownership
    system("chown user1 user1_readonly.txt");
    system("chown user2 user2_readwrite.txt");
    system("chown user3 user3_executeonly.txt");
    printf("Ownership transferred to users.\n");

    //Checking the permissions
    //Condition: user1 has read-only. user2 has no permission to user1's file.
    printf("Simulating permission checks...\n");
    printf("Testing what happens when 'user2' trys to modify 'user1_readonly.txt':\n");

    //We try to get user2 to append ("echo") text into user1's file.
    int status = system("runuser -u user2 -- bash -c \"echo 'Hack' >> user1_readonly.txt\" 2>&1");
    
    if (status != 0) {
        printf("\nAccess Denied. The operation failed.\n");
    }

    return 0;
}
