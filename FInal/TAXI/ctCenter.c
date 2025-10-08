#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define EXITCMD 0
#define WORCKCMD 90
#define STATUSCMD 113

int pausetime = 10;
#define MAXDRIVERS 5

int driverscount = 0;

struct driver {
    pid_t iPID;
    int ctToTaxifd[2];
    int taxiToCtfd[2];
};

struct driver drivers[MAXDRIVERS];

void *timerDriver(void *arg) {
    int *statusPointer = (int *) arg;
    sleep(pausetime);
    *statusPointer = 1;
    printf("Driver says: gotov rabotat\n");
    return NULL;
}



//надеюсь ничего не сломал при переносе. Проверил, ВРОДЕ ТАК-ЖЕ работает.
void createDriver() {
    if (driverscount >= MAXDRIVERS) {
        printf("ERROR: Too many drivers\n");
        return;
    }

    if (pipe(drivers[driverscount].ctToTaxifd) == -1 || pipe(drivers[driverscount].taxiToCtfd) == -1) {
        perror("pipe");
        return;
    }

    drivers[driverscount].iPID = fork();
    if (drivers[driverscount].iPID == -1) {
        perror("fork");
        return;
    }

    //такси
    if (drivers[driverscount].iPID == 0) {
        close(drivers[driverscount].ctToTaxifd[1]);
        close(drivers[driverscount].taxiToCtfd[0]);

        printf("Driver says: im ready, №-%d with pid-%d\n", driverscount, getpid());
        int Avalible = 1;

        // Ждем задания
        while (1) {
            int task;
            if (read(drivers[driverscount].ctToTaxifd[0], &task, sizeof(int)) <= 0) {
                // Если канал закрылся, завершаем процесс
                break;
            }

            if (task == EXITCMD) {
                break;
            }

            if (task == STATUSCMD) {
                write(drivers[driverscount].taxiToCtfd[1], &Avalible, sizeof(int));
            }

            if (task == WORCKCMD) {
                write(drivers[driverscount].taxiToCtfd[1], &Avalible, sizeof(int));
                if (Avalible == 0) {
                    printf("Driver says: im busy\n");
                }
                if (Avalible == 1) {
                    Avalible = 0;
                    pthread_t timer;
                    if (pthread_create(&timer, NULL, timerDriver, &Avalible) == -1) {
                        perror("pthread_create");
                    }
                    pthread_detach(timer);
                }
            }
        }
        close(drivers[driverscount].ctToTaxifd[0]);
        close(drivers[driverscount].taxiToCtfd[1]);
        exit(EXIT_SUCCESS);
    }

    //диспетчерская
    close(drivers[driverscount].ctToTaxifd[0]);
    close(drivers[driverscount].taxiToCtfd[1]);
    printf("Created a new driver, №-%d with pid-%d\n", driverscount, drivers[driverscount].iPID);
    driverscount++;
}

void sendTask() {
    if (driverscount == 0) {
        printf("What Artem? No drivers created yet\n");
        return;
    }

    int dchoise;
    printf("Enter driver number (0 - %d): ", driverscount - 1);
    while (1) {
        if (scanf("%d", &dchoise) != 1 || dchoise < 0 || dchoise >= driverscount) {
            printf("Wrong choice\n");
            while (getchar() != '\n'); // Очистка буфера ввода
            continue;
        }
        break;
    }

    int signal = WORCKCMD;
    write(drivers[dchoise].ctToTaxifd[1], &signal, sizeof(int));
    read(drivers[dchoise].taxiToCtfd[0], &signal, sizeof(int));

    if (signal == 0) {
        printf("BUSY\n");
        return;
    }
    printf("Opyat vkalivat...\n");
}

void getStatus() {
    if (driverscount == 0) {
        printf("What Artem? No drivers created yet\n");
        return;
    }

    int dchoise;
    printf("Enter driver number (0 - %d):", driverscount - 1);
    while (1) {
        if (scanf("%d", &dchoise) != 1 || dchoise < 0 || dchoise >= driverscount) {
            printf("Wrong choice\n");
            while (getchar() != '\n');
            continue;
        }
        break;
    }

    int signal = STATUSCMD;
    write(drivers[dchoise].ctToTaxifd[1], &signal, sizeof(int));
    read(drivers[dchoise].taxiToCtfd[0], &signal, sizeof(int));

    if (signal == 1) {
        printf("Stasus: AVALIBLE, pid: %d\n", drivers[dchoise].iPID);
    } else {
        printf("Stasus: BUSY, pid: %d\n", drivers[dchoise].iPID);
    }
}

void getAllDriversStatus() {
    for (int i = 0; i < driverscount; i++) {
        int signal = STATUSCMD;
        write(drivers[i].ctToTaxifd[1], &signal, sizeof(int));
        read(drivers[i].taxiToCtfd[0], &signal, sizeof(int));
        if (signal == 1) {
            printf("Stasus: AVALIBLE, pid: %d\n\n", drivers[i].iPID);
        } else {
            printf("Stasus: BUSY, pid: %d\n\n", drivers[i].iPID);
        }
    }
}

void DoExit() {
    printf("Exiting...\n");
    int signal = EXITCMD;
    for (int i = 0; i < driverscount; i++) {
        write(drivers[i].ctToTaxifd[1], &signal, sizeof(int));
        close(drivers[i].ctToTaxifd[1]); // Закрываем пайпы после отправки команды
        close(drivers[i].taxiToCtfd[0]);
    }
    for (int i = 0; i < driverscount; i++) {
        waitpid(drivers[i].iPID, NULL, 0);
    }
    exit(EXIT_SUCCESS);
}

int main() {
    printf("Enter pause duration: ");
    scanf("%d", &pausetime);

    printf("CLI:\n 1: create_driver \n 2: send_task \n 3: get_status \n 4: get_drivers \n 5: exit\n");

    int choice;
    while (1) {
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
            printf("Wrong choice\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                createDriver();
                break;
            case 2:
                sendTask();
                break;
            case 3:
                getStatus();
                break;
            case 4:
                getAllDriversStatus();
                break;
            case 5:
                DoExit();
                break;
        }
    }
    return EXIT_SUCCESS;
}
