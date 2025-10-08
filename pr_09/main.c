#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define alot 180000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rwlock;
pthread_spinlock_t spinlock;

int glNum = 0, glNumSafe = 0;

void threadId() {
    printf("%d\n", pthread_self());
}

void ink(int *num) {
    for (int i = 1; i <= alot; i++) {
        *num = *num + 1;
    }
}

void speed() {
    glNum = glNum + 5;
}

void speedSafe() {
    pthread_mutex_lock(&mutex);
    glNumSafe = glNumSafe + 5;
    pthread_mutex_unlock(&mutex);
}

void speedSafeSpinLock() {
    pthread_spin_lock(&spinlock);
    glNumSafe = glNumSafe + 5;
    pthread_spin_unlock(&spinlock);
}

void speedSafeRW() {
    int localSum = 0;
    localSum++;
    pthread_rwlock_wrlock(&rwlock);
    glNumSafe = glNumSafe + localSum;
    pthread_rwlock_unlock(&rwlock);
}

int main() {
    printf("Task 1\n");
    pthread_t threads[5];
    for (int i = 0; i <= 5; i++) {
        pthread_create(&threads,NULL, &threadId,NULL);
    }
    int b = 0;
    sleep(1);
    //
    printf("Task 2.1\n");
    printf("%d\n", b);
    ink(&b);
    printf("%d\n", b);
    //
    printf("Task 2.2\n");
    pthread_t threadsGonka[2];
    pthread_create(&threadsGonka[0],NULL, &speed,NULL);
    pthread_create(&threadsGonka[1],NULL, &speed,NULL);
    printf("%d\n", glNum);
    //
    printf("Task 2.3\n");
    pthread_t threadsGonkaSafe[2];
    pthread_create(&threadsGonkaSafe[0],NULL, &speedSafe,NULL);
    pthread_create(&threadsGonkaSafe[1],NULL, &speedSafe,NULL);
    pthread_join(threadsGonkaSafe[0], NULL);
    pthread_join(threadsGonkaSafe[1], NULL);
    printf("%d\n", glNumSafe);
    //
    glNumSafe = 0;
    printf("Task 2.4 incorrect\n");
    pthread_t threadsGonkaSafeLock[2];
    pthread_create(&threadsGonkaSafeLock[0],NULL, &speedSafeSpinLock,NULL);
    pthread_create(&threadsGonkaSafeLock[1],NULL, &speedSafeSpinLock,NULL);
    printf("%d\n", glNumSafe);
    //
    glNumSafe = 0;
    printf("Task 2.5\n");
    pthread_t threadsGonkaSafeRW[2];
    pthread_create(&threadsGonkaSafeRW[0],NULL, &speedSafeRW,NULL);
    pthread_create(&threadsGonkaSafeRW[1],NULL, &speedSafeRW,NULL);
    pthread_join(threadsGonkaSafeRW[0], NULL);
    pthread_join(threadsGonkaSafeRW[1], NULL);
    printf("%d\n", glNumSafe);
    //
    printf("Task 3\n");

    //
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("Ошибка pthread_mutex_destroy");
        return 1;
    }

    return EXIT_SUCCESS;
}
