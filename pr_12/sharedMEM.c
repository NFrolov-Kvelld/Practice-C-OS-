#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHM_NAME "/chat_shm"
#define SEM_PARENT_NAME "/chat_sem_parent"
#define SEM_CHILD_NAME "/chat_sem_child"
#define BUFFER_SIZE 256

typedef struct {
    char message[BUFFER_SIZE];
    int is_parent_writing;
    int is_child_writing;
} shared_data;

void parent_process(shared_data *data, sem_t *sem_parent, sem_t *sem_child) {
    char buffer[BUFFER_SIZE];

    printf("Child proces is started\n");

    while (1) {
        printf("Parent. Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';


        sem_wait(sem_parent);

        strncpy(data->message, buffer, BUFFER_SIZE);
        data->is_parent_writing = 1;


        sem_post(sem_child);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        sem_wait(sem_parent);
        if (data->is_child_writing) {
            printf("Child proces: %s\n", data->message);
            data->is_child_writing = 0;
        }
        sem_post(sem_parent);

        if (strcmp(data->message, "exit") == 0) {
            break;
        }
    }
}

void child_process(shared_data *data, sem_t *sem_parent, sem_t *sem_child) {
    char buffer[BUFFER_SIZE];

    printf("Child proces started\n");

    while (1) {
        sem_wait(sem_child);
        if (data->is_parent_writing) {
            printf("Parentr proces: %s\n", data->message);
            data->is_parent_writing = 0;
        }
        sem_post(sem_child);

        if (strcmp(data->message, "exit") == 0) {
            break;
        }


        printf("Child. Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        sem_wait(sem_child);
        strncpy(data->message, buffer, BUFFER_SIZE);
        data->is_child_writing = 1;
        sem_post(sem_parent);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(shared_data))) {
        exit(EXIT_FAILURE);
    }
    shared_data *data = mmap(NULL, sizeof(shared_data),
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        exit(EXIT_FAILURE);
    }

    memset(data, 0, sizeof(shared_data));
    data->is_parent_writing = 0;
    data->is_child_writing = 0;
    sem_t *sem_parent = sem_open(SEM_PARENT_NAME, O_CREAT, 0666, 0);
    sem_t *sem_child = sem_open(SEM_CHILD_NAME, O_CREAT, 0666, 0);
    if (sem_parent == SEM_FAILED || sem_child == SEM_FAILED) {
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        parent_process(data, sem_parent, sem_child);
        wait(NULL);
    } else {
        child_process(data, sem_parent, sem_child);
        exit(EXIT_SUCCESS);
    }
    sem_close(sem_parent);
    sem_close(sem_child);
    sem_unlink(SEM_PARENT_NAME);
    sem_unlink(SEM_CHILD_NAME);
    munmap(data, sizeof(shared_data));
    shm_unlink(SHM_NAME);

    printf("chat is done.\n");
    return 0;
}
