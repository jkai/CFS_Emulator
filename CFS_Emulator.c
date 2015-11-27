#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define COUSUMER_NUMBER 4

void generate_producer(void);
void generate_consumers(void);
void *producer_thread_function(void *arg);
void *consumer_threads_function(void *arg);

int main() {

    generate_producer();
    generate_consumers();

    printf("All done\n");
    exit(EXIT_SUCCESS);
}

void generate_producer(void)
{
    int res;
    pthread_t producer_thread;

    //Generate producer thread
    res = pthread_create(&producer_thread, NULL, producer_thread_function, NULL);
    if (res != 0) {
        perror("Thread producer creation failed");
        exit(EXIT_FAILURE);
    }
}

void generate_consumers(void)
{
    int res;
    pthread_t consumer_threads[COUSUMER_NUMBER];
    int consumer_threads_index;

    for(consumer_threads_index = 0; consumer_threads_index < COUSUMER_NUMBER; consumer_threads_index++) {
        res = pthread_create(&(consumer_threads[consumer_threads_index]), NULL, consumer_threads_function, (void *)consumer_threads_index);
        if (res != 0) {
            perror("Thread consumer creation failed");
            exit(EXIT_FAILURE);
        }
    }
}

void *producer_thread_function(void *arg) {
    printf("[Producer] Creation successful!\n");
    pthread_exit("Producer quitting...\n");
}

void *consumer_threads_function(void *arg) {
    printf("[Consumer %d] Creation successful!\n", arg);
    pthread_exit("Consumer quitting...\n");
}


