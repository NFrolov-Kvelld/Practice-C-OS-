#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int parent_to_child[2];
    int child_to_parent[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        printf("Child is ready:\n");

        while (1) {
            ssize_t count = read(parent_to_child[0], buffer, BUFFER_SIZE);
            if (count > 0) {
                buffer[count] = '\0';
                printf("Parent message: %s", buffer);
            } else if (count == 0) {
                break;
            }

            printf("Child: ");
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            write(child_to_parent[1], buffer, strlen(buffer));
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(EXIT_SUCCESS);
    }
        close(parent_to_child[0]);
        close(child_to_parent[1]);

        printf("Parent is ready:\n");

        while (1) {
            printf("parent: ");
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            write(parent_to_child[1], buffer, strlen(buffer));

            ssize_t count = read(child_to_parent[0], buffer, BUFFER_SIZE);
            if (count > 0) {
                buffer[count] = '\0';
                printf("Child message: %s", buffer);
            } else if (count == 0) {
                break;
            }
        }

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        wait(NULL);
        exit(EXIT_SUCCESS);
}