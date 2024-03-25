//
// Created by Emir Tomrukcu on 24.03.2024.
//
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define READER_COUNT 2
#define WRITER_COUNT 2
#define BOOK_COUNT 3
#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];  	/* shared buffer */
int add = 0;  			/* place to add next element */
int rem = 0;  			/* place to remove next element */
int num = 0;  			/* number elements in buffer */
int shared = 0;         /* when shared > 0, shared number of readers are reading. when shared = -1, a writer is writing*/

pthread_mutex_t mutex = PTHREAD_DEFAULT_MUTEX_INITIALIZER;
pthread_cond_t readCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writeCond = PTHREAD_COND_INITIALIZER;

void *reader(void *param);
void *writer(void *param);

int main(){
    pthread_t readers[READER_COUNT];
    pthread_t writers[WRITER_COUNT];

    int reader_id[READER_COUNT];
    int writer_id[WRITER_COUNT];

    srand(time(0));

    for (int i = 0; i < READER_COUNT; i++){
        reader_id[i-1] = i;
        pthread_create(&readers[i], NULL, reader, &reader_id[i-1]);
    }

    for (int i = 0; i < WRITER_COUNT; i++){
        writer_id[i-1] = i;
        pthread_create(&writers[i], NULL, writer, &writer_id[i-1]);
    }

    for (int i = 0; i < READER_COUNT; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < WRITER_COUNT; i++)
        pthread_join(writers[i], NULL);

    return 0;
}

void *reader(void *param){
    int *p = (int*)param;
    int ID = *p;
    int count = 0;
    int curr;
    int currentShared;
    while (count < BOOK_COUNT){
        usleep(1000 * (rand() % READER_COUNT + WRITER_COUNT));
        pthread_mutex_lock(&mutex);
            while (num == 0 && shared >= 0)
                pthread_cond_wait(&readCond, &mutex);
            shared++;
        pthread_mutex_unlock(&mutex);

        curr = buffer[rem];
        rem = (rem+1) % BUFFER_SIZE;
        num--;
        count++;
        currentShared = shared;

        pthread_mutex_lock(&mutex);
            printf("Reader %d read the value %d when there are %d readers\n", ID, curr, currentShared);
            fflush(stdout);
            shared--;
            if (num == 0)
                pthread_cond_broadcast(&writeCond);
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}

void *writer(void *param){
    int *p = (int*)param;
    int ID = *p;
    int curr = 0;
    int currentShared;
    while (curr < BOOK_COUNT){
        usleep(1000*rand()%WRITER_COUNT+READER_COUNT);
        pthread_mutex_lock(&mutex);
            while (shared != 0)
                pthread_cond_wait(&writeCond, &mutex);
            shared--;
        pthread_mutex_unlock(&mutex);

        buffer[add] = ID*BOOK_COUNT + curr;
        add = (add+1) % BUFFER_SIZE;
        num++;
        curr++;
        currentShared = -shared;

        pthread_mutex_lock(&mutex);
            shared = 0;
            printf("Writer %d wrote the value %d when there are %d writers\n", ID, ID*BOOK_COUNT + curr-1, currentShared);
            fflush(stdout);
            pthread_cond_broadcast(&readCond);
            pthread_cond_broadcast(&writeCond);
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}
