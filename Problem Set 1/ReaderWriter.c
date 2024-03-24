//
// Created by Emir Tomrukcu on 24.03.2024.
//
#include <stdio.h>
#include <pthread.h>

#define READER_COUNT 5
#define WRITER_COUNT 5
#define BOOK_COUNT 30
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

    for (int i = 1; i <= READER_COUNT; i++){
        reader_id[i-1] = i;
        pthread_create(&readers[i], NULL, reader, &reader_id[i-1]);
    }

    for (int i = 1; i <= WRITER_COUNT; i++){
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
    while (count < BOOK_COUNT){
        pthread_mutex_lock(&mutex);
            if (num < 0) // underflow
                exit(1);
            while (num == 0)
                pthread_cond_wait(&readCond, &mutex);

            curr = buffer[rem];
            rem = (rem+1) % BUFFER_SIZE;
            num--;
            count++;
        pthread_mutex_unlock(&mutex);

        printf("Reader %d read the value %d when there are %d readers", ID, curr, shared);
        fflush(stdout);
    }
}

void *writer(void *param){

}
