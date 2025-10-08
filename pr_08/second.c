#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void giveInfo() {
    printf("PID: %d, PPID: %d\n", getpid(), getppid());
}

int main() {
    printf("Main process started - ");
    printf("PID :%d\n", getpid());
    //1st subParent
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork 1 is failed");
    }
    else if (pid1 == 0) {
        //3rd
        pid_t pid3 = fork();
        if (pid3 == -1) {
            perror("fork 3 is failed");
        }
        else if (pid3 == 0) {
            giveInfo();
            exit(EXIT_SUCCESS);
        }

        //4th
        pid_t pid4 = fork();
        if (pid4 == -1) {
            perror("fork 3 is failed");
        }
        else if (pid4 == 0) {
            giveInfo();
            exit(EXIT_SUCCESS);
        }

        waitpid(pid3,NULL, 0);
        waitpid(pid4,NULL, 0);
        giveInfo();
        exit(EXIT_SUCCESS);
    }
    // 2nd subParent
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork 2 is failed");
    }
    else if (pid2 == 0) {
        //5th
        pid_t pid5 = fork();
        if (pid5 == -1) {
            perror("fork 5 is failed");
        }
        else if (pid5 == 0) {
            giveInfo();
            exit(EXIT_SUCCESS);
        }
        waitpid(pid5,NULL, 0);
        giveInfo();
        exit(EXIT_SUCCESS);
    }
    waitpid(pid1,NULL, 0);
    waitpid(pid2,NULL, 0);
    printf(" main fork end\n");
    giveInfo();
    return EXIT_SUCCESS;
}