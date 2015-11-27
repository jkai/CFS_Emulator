#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void generate_producer(void);
void *producer_thread_fuction(void *arg);

int main() {

    generate_producer();
    //generate_consumers();

    printf("All done\n");
    exit(EXIT_SUCCESS);
}

void generate_producer(void)
{
    int res;
    pthread_t producer_thread;
    void *thread_result;

    //Generate producer thread
    res = pthread_create(&producer_thread, NULL, producer_thread_fuction, NULL);
    if (res != 0) {
        perror("Thread producer creation failed");
        exit(EXIT_FAILURE);
    }
    
}

void *producer_thread_fuction(void *arg) {
    printf("[Producer] Creation successful!\n");
    pthread_exit("Producer quitting...\n");
}