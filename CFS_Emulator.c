/* 
SYSC4001 Assignment3
Student: Junjie Kai	100814819
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "Common.h"

void generate_producer(void);
void generate_consumers(void);
void *producer_thread_function(void *arg);
void *consumer_threads_function(void *arg);
void generate_items(void);
void wait_for_producer(void);
void clean_up_and_quit(void);

/* Static variables */
static int processes_number;		//User defined processes number, default = 20
static multilevel_queue cpu_queues[CORE_NUMBER];	//4 Multilevel queues for 4 cores
static pthread_t producer_thread;	//One producer thread
static pthread_t consumer_threads[CORE_NUMBER];		//4 Consumer threads as 4 cores

int main(int argc, char *argv[])
{
	/* Get processes_number from user, use 20 if no input. */
	if (argc != 2) {
		processes_number = DEFAULT_PROCESS_NUMBER;
		printf("Using default processes_number: 20.\n");
	} else {
		processes_number = atoi(argv[1]);
		printf("Using user defined processes_number: %d.\n", processes_number);
	}
	
	/* Generate producer thread and consumer threads*/
    generate_producer();
    generate_consumers();
	

	/* Wait for threads */
	wait_for_producer();
    /* Clean up before quitting */
	clean_up_and_quit();
}

void generate_producer(void)
{
    int res;
	
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
    long int consumer_threads_index;

    //Generate consumer threads
    for(consumer_threads_index = 0; consumer_threads_index < CORE_NUMBER; consumer_threads_index++) {
        res = pthread_create(&(consumer_threads[consumer_threads_index]), NULL, consumer_threads_function, (void *)consumer_threads_index);
        if (res != 0) {
            perror("Thread consumer creation failed");
            exit(EXIT_FAILURE);
        }
    }
}

void *producer_thread_function(void *arg)
{
    printf("[Producer] Creation successful!\n");
    generate_items();
    pthread_exit("Producer quitting...\n");
}

void *consumer_threads_function(void *arg)
{
    long int i = (long int)arg;
    printf("[Consumer %li] Creation successful!\n", i);
    pthread_exit("Consumer quitting...\n");
}

void generate_items(void)
{

}

void wait_for_producer(void)
{
	int res = pthread_join(producer_thread, NULL);
	if( res != 0) {
		perror("pthread_join failed");
	}
}

void clean_up_and_quit(void)
{
	printf("All done, cleaning up...\n");
    exit(EXIT_SUCCESS);
}




