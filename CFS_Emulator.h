#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define COUSUMER_NUMBER 4

void generate_producer(void);
void generate_consumers(void);
void *producer_thread_function(void *arg);
void *consumer_threads_function(void *arg);
void generate_items(void);