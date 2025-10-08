#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child process: PID = %d, PPID = %d\n", getpid(), getppid());
        exit(EXIT_SUCCESS);
    }
    //в родительском
    printf("%d\n",pid);
    printf("Parent process: PID = %d, PPID = %d\n", getpid(), getppid());

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("Child process exited with status: %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process terminated abnormally\n");
    }


    return EXIT_SUCCESS;
}
