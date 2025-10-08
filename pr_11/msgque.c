#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/stat.h>

#define QUEUE_NAME_PARENT "/chat_queue_parent"
#define QUEUE_NAME_CHILD "/chat_queue_child"
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

void parent_process() {
    mqd_t mq_parent, mq_child;
    struct mq_attr attr;
    char buffer[MSG_BUFFER_SIZE];
    int running = 1;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    mq_parent = mq_open(QUEUE_NAME_PARENT, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq_parent == (mqd_t)-1) {
        perror("Parent: mq_open (parent)");
        exit(EXIT_FAILURE);
    }
    mq_child = mq_open(QUEUE_NAME_CHILD, O_WRONLY);
    if (mq_child == (mqd_t)-1) {
        perror("Parent: mq_open (child)");
        mq_close(mq_parent);
        mq_unlink(QUEUE_NAME_PARENT);
        exit(EXIT_FAILURE);
    }

    printf("Parent is start. Exit to.... exit\n");

    while (running) {
        printf("Parent message");
        fgets(buffer, MAX_MSG_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (mq_send(mq_child, buffer, strlen(buffer) + 1, 0))
            {
            perror("Parent: mq_send");
            break;
        }

        if (strcmp(buffer, "exit") == 0) {
            running = 0;
            break;
        }
        ssize_t bytes_read = mq_receive(mq_parent, buffer, MSG_BUFFER_SIZE, NULL);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            printf("child proces: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) {
                running = 0;
            }
        } else {
            perror("Parent: mq_receive");
            break;
        }
    }
    mq_close(mq_parent);
    mq_close(mq_child);
    mq_unlink(QUEUE_NAME_PARENT);
    printf("Parent is done\n");
}

void child_process() {
    mqd_t mq_parent, mq_child;
    struct mq_attr attr;
    char buffer[MSG_BUFFER_SIZE];
    int running = 1;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    mq_child = mq_open(QUEUE_NAME_CHILD, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq_child == (mqd_t)-1) {
        perror("Child: mq_open (child)");
        exit(EXIT_FAILURE);
    }
    mq_parent = mq_open(QUEUE_NAME_PARENT, O_WRONLY);
    if (mq_parent == (mqd_t)-1) {
        perror("Child: mq_open (parent)");
        mq_close(mq_child);
        mq_unlink(QUEUE_NAME_CHILD);
        exit(EXIT_FAILURE);
    }

    printf("Child is starte. Exit to.... exit\n");

    while (running) {
        ssize_t bytes_read = mq_receive(mq_child, buffer, MSG_BUFFER_SIZE, NULL);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            printf("Parent process: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) {
                running = 0;
                break;
            }
        } else {
            break;
        }
        printf("child. Enter message: ");
        fgets(buffer, MAX_MSG_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (mq_send(mq_parent, buffer, strlen(buffer) + 1, 0)) {
            perror("Child: mq_send");
            break;
        }

        if (strcmp(buffer, "exit") == 0) {
            running = 0;
        }
    }

    mq_close(mq_child);
    mq_close(mq_parent);
    mq_unlink(QUEUE_NAME_CHILD);
    printf("child is done\n");
}

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        parent_process();
        wait(NULL);
    } else {
        child_process();
    }

    return 0;
}