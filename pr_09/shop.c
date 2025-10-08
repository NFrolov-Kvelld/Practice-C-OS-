#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN_DELAY 1
#define MAX_DELAYC 4
#define MAX_DELAYW 3
#define ROOMS 6
#define CAPACITY 10;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //надо бы создать несколько, но и 1 пойдет
int shopOpen = 1;
struct Shop {
    int *assort;
};
struct Costomer {
    int need;
    struct Shop *shop;
};
struct Worker {
    struct Shop *shop;
};

void customerThread(void* arg) {
    struct Costomer costomer = *(struct Costomer*)arg;
    while (costomer.need > 0) {

        int delay = rand() % MAX_DELAYC + MIN_DELAY;
        sleep(delay);
        int goRoom = rand() % ROOMS;
        pthread_mutex_lock(&mutex);
        printf("Costomer empted room %d. Need: %d\n", goRoom, costomer.need);
        costomer.need = costomer.need - CAPACITY;
        costomer.shop->assort[goRoom] = 0;
        for (int i = 0;i<ROOMS;i++) {
            printf("%d\t", costomer.shop->assort[i]);
        }
        printf("\n");
        pthread_mutex_unlock(&mutex);
    }
    printf("Costomer`s needs are pleasured\n");
    sleep(5);
    shopOpen = 0;
}

void WorkerThread(void* arg) {
    while (shopOpen == 1) {
        struct Worker worker = *(struct Worker*)arg;
        int delay = rand() % MAX_DELAYW + MIN_DELAY;
        sleep(delay);
        int goRoom = rand() % ROOMS;
        pthread_mutex_lock(&mutex);
        printf("Worked filled up room %d\n", goRoom);
        worker.shop->assort[goRoom] = CAPACITY;
        for (int i = 0;i<ROOMS;i++) {
            printf("%d\t", worker.shop->assort[i]);
        }
        printf("\n");
        pthread_mutex_unlock(&mutex);
    }
    printf("Shift is done\n");
}

int main() {
    srand(time(NULL));
    pthread_t Take;
    pthread_t Set;
    int arr[ROOMS];
    for (int i =0; i < ROOMS; i++) {
        arr[i] = CAPACITY;
    }
    struct Shop shop = {arr};
    struct Costomer costomer = {100,&shop};
    struct Worker worker = {&shop};
    pthread_create(&Set, NULL, &WorkerThread, &worker);
    pthread_create(&Take, NULL, &customerThread, &costomer);

    pthread_join(Set, NULL);
    pthread_join(Take, NULL);

    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
