/* 
SYSC4001 Assignment3
Student: Junjie Kai	100814819
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "Common.h"
#include "cpu_queue.h"

void generate_producer(void);
void generate_consumers(void);
void generate_balancer(void);
void *producer_thread_function(void *arg);
void *balancer_thread_function(void *arg);
void *consumer_threads_function(void *arg);
void generate_items(void);
void consume_processes(int core_num);
void initial_cpu_queues(void);
void balance_queues(void);
void wait_for_producer(void);
void wait_for_consumers(void);
void print_all_queues(void);
void clean_up_and_quit(void);

/* Static variables */
static int processes_number;							//User defined processes number, default = 20
static int done_flag;									//Tells balancer to stop
static int alive_consumers;								//# of alive consumers
static multilevel_queue cpu_queues[CORE_NUMBER];		//4 Multilevel queues for 4 cores
static pthread_t producer_thread;						//One producer thread
static pthread_t balancer_thread;						//One balancer thread
static pthread_t consumer_threads[CORE_NUMBER];			//4 Consumer threads as 4 cores
static pthread_mutex_t mutex;							//mutex for P/C
static pthread_mutex_t consumer_mutexes[CORE_NUMBER];	//4 mutex for balancer

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

	done_flag = 0;		//Not done
	alive_consumers = 0;//No alive consumer
	
	/* Create mutex */
	if ( pthread_mutex_init(&mutex, NULL) != 0) {
		printf("Failed to create mutex\n");
		exit(EXIT_FAILURE);
	}

	/* Create consumer mutex */
	int i;
	for(i = 0; i < CORE_NUMBER; ++i)
	{
		if ( pthread_mutex_init(&consumer_mutexes[i], NULL) != 0) {
			printf("Failed to create mutex\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Seed the random number generater with pid */
	srand(getpid());
	
	/* Generate producer thread*/
    generate_producer();
	
	/* Wait for producer to produce */
	wait_for_producer();
	
	/* Generate consumer threads*/
	generate_consumers();

	/* Generate balancer thread*/
	generate_balancer();
	
    /* Wait for Consumers */
	wait_for_consumers();
	
    /* Clean up before quitting */
	clean_up_and_quit();
}

void wait_for_consumers(void)
{
	int i;
	for(i = 0; i < CORE_NUMBER; ++i)
	{
		int res = pthread_join(consumer_threads[i], NULL);
		if( res != 0) {
			perror("pthread_join failed");
		}
	}
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

void generate_balancer(void)
{
    int res;
	
    //Generate producer thread
    res = pthread_create(&balancer_thread, NULL, balancer_thread_function, NULL);
    if (res != 0) {
        perror("Thread balancer creation failed");
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
        alive_consumers++;
    }
}

void *producer_thread_function(void *arg)
{
    printf("[Producer] Creation successful! Generating processes...\n");
    generate_items();
    pthread_exit("Producer quitting...\n");
}

void *balancer_thread_function(void *arg)
{
    printf("[Balancer] Creation successful!\n");
    //Wake up every 3s to balance the queues
    while (!done_flag)
    {
    	balance_queues();
		sleep(3);
    }

    pthread_exit("Balancer quitting...\n");
}

void *consumer_threads_function(void *arg)
{
	/* Initializing consumer_thread */
    long int int_buf = (long int)arg;
	int core_num = (int)int_buf;
	
	/* Consume the processes */
	consume_processes(core_num);

    pthread_exit("Consumer quitting...\n");
}

void consume_processes(int core_num)
{
	/* Point to the core's own multilevel_queue */
	multilevel_queue* mq = &(cpu_queues[core_num]);
	
	while (!multilevel_queue_empty(mq))
	{
		//Lock its own cpu queue
		pthread_mutex_lock(&consumer_mutexes[core_num]);

		/* Assign the pointers */
		run_queue* rq0 = &(mq->rq0);
		run_queue* rq1 = &(mq->rq1);
		run_queue* rq2 = &(mq->rq2);
		
		/* Start from RQ0 */
		if (!run_queue_empty(rq0))
		{
			fetch_queue(rq0, core_num);
		}
		/* If RQ0 done, go RQ1 */
		else if (!run_queue_empty(rq1))
		{
			fetch_queue(rq1, core_num);
		}
		/* If RQ1 done, go RQ2 */
		else if (!run_queue_empty(rq2))
		{
			fetch_queue(rq2, core_num);
		}
		else
		{
			
		}
		/* Release the mutex */
		pthread_mutex_unlock(&consumer_mutexes[core_num]);
	}
	
}

void generate_items(void)
{
	int i;			//Index
	int core_num;		//Define which core to assign the generated processes
	process_struct *current_process	= NULL;	//Point to the process in queue
	
	/* Mutex */
	pthread_mutex_lock(&mutex);
	
	initial_cpu_queues();
	/* 
		Processes generation ratios:
		- SCHEDULE_FIFO		20%
		- SCHEDULE_RR		20%
		- SCHEDULE_NORMAL	60%
	*/
	for (i = 0; i < processes_number; ++i)
	{
		core_num = i % 4;	//Assign the generated process from Core0 to Core3
		/* 0~2 -> Normal(60%), 3 -> RR(20%), 4 -> FIFO(20%) */
		switch (i % 5)
		{
			/* Normal */
			case 0: case 1: case 2:			
			/* Update the RQ1 queue */
				//Assign the generated process to tail, and update the tail
				current_process = &(cpu_queues[core_num].rq1.processes[cpu_queues[core_num].rq1.tail++]);
				
			/* Update the process */
				//Assign the process id = i + 1, which means pid starts from 1
				current_process->pid = i + 1;
				//Assign the process type as SCHEDULE_NORMAL
				current_process->schedule_type = SCHEDULE_NORMAL;
				//priority = 120 (Default)
				current_process->priority = DEFAULT_STATIC_PRIORITY;
				//expected_exec_time = n * 100ms, n = [2, 20]
				current_process->expected_exec_time = (2 + rand() % 18) * 100;
				//Default time_slice = 100ms
				current_process->time_slice = DEFAULT_TIME_SLICE;
				//Not finished
				current_process->finished = 0;
				//Get times
				gettimeofday(&current_process->started_time, NULL);
				gettimeofday(&current_process->last_sleep_time, NULL);
				//Done
				break;

			/* RR */
			case 3:
			/* Update the RQ0 queue */
				//Assign the generated process to tail, and update the tail
				current_process = &(cpu_queues[core_num].rq0.processes[cpu_queues[core_num].rq0.tail++]);
					
			/* Update the process */
				//Assign the process id = i + 1, which means pid starts from 1
				current_process->pid = i + 1;
				//Assign the process type as SCHEDULE_RR
				current_process->schedule_type = SCHEDULE_RR;
				//priority = [0, 100) (Static)
				current_process->priority = rand() % 100;
				//expected_exec_time = n * 800ms, n = [1, 5]
				current_process->expected_exec_time = (1 + (rand() % 5)) * 800;
				//Default time_slice = 100ms
				current_process->time_slice = ((140 - current_process->priority) * 5);
				//Not finished
				current_process->finished = 0;
				//Get time
				gettimeofday(&current_process->started_time, NULL);
				//Done
			break;
			/* FIFO */
			case 4:
			/* Update the RQ0 queue */
				//Assign the generated process to tail, and update the tail
				current_process = &(cpu_queues[core_num].rq0.processes[cpu_queues[core_num].rq0.tail++]);
					
			/* Update the process */
				//Assign the process id = i + 1, which means pid starts from 1
				current_process->pid = i + 1;
				//Assign the process type as SCHEDULE_FIFO
				current_process->schedule_type = SCHEDULE_FIFO;
				//priority = [0, 100) (Static)
				current_process->priority = rand() % 100;
				//expected_exec_time = n * 800ms, n = [1, 5]
				current_process->expected_exec_time = (1 + (rand() % 5)) * 800;
				//Default time_slice = expected_exec_time
				current_process->time_slice = current_process->expected_exec_time;
				//Not finished
				current_process->finished = 0;
				//Get time
				gettimeofday(&current_process->started_time, NULL);
				//Done
			break;
			default:
				fprintf(stderr, "Invalid process type, quitting...\n");
				exit(EXIT_FAILURE);	
		}
		
		
	}
	/* Release the mutex */
	pthread_mutex_unlock(&mutex);
	printf("[Producer] Processes are generated, the current status is:\n");
	print_all_queues();

}


void initial_cpu_queues(void)
{
	int i;
	printf("[Producer] Initializing cpu queues...\n");
	multilevel_queue* mq = NULL;
	
	for (i = 0; i < CORE_NUMBER; ++i)
	{
		mq = &(cpu_queues[i]);
		multilevel_queue_init(mq);
	}
	
	printf("[Producer] CPU queues are initialized.\n");
}

void wait_for_producer(void)
{
	int res = pthread_join(producer_thread, NULL);
	if( res != 0) {
		perror("pthread_join failed");
	}
}

void balance_queues(void)
{
	int core_index;
	int count;
	int min_count = RUN_QUEUE_SIZE;
	int max_count = 0;
	int min_index = 0;
	int max_index = 0;

	//Lock Consumer threads, hard coded them for better performance, to avoid branches.
	pthread_mutex_lock(&consumer_mutexes[0]);
	pthread_mutex_lock(&consumer_mutexes[1]);
	pthread_mutex_lock(&consumer_mutexes[2]);
	pthread_mutex_lock(&consumer_mutexes[3]);

	//Go through four cores
	for (core_index = 0; core_index < CORE_NUMBER; ++core_index)
	{
		count = (cpu_queues[core_index].rq0.tail - cpu_queues[core_index].rq0.head) + (cpu_queues[core_index].rq1.tail - cpu_queues[core_index].rq1.head) + (cpu_queues[core_index].rq2.tail - cpu_queues[core_index].rq2.head);
		printf("[Balancer] Core%d has %d processes in its queues\n", core_index, count);	
		if (min_count >= count)
		{
			min_count = count;
			min_index = core_index;
		}
		else if (max_count <= count)
		{
			max_count = count;
			max_index = core_index;
		}
	}

	if (max_count == 0)
	{
		printf("[Balancer] No need to balance this time.\n");
		done_flag = 1;
	}
	else {
		//Check if balance necesery
		if ((max_count - min_count) >= 2)
		{
			/* Move from CPU[max_index] to CPU[min_index] */
			cpu_queues[min_index].rq1.processes[cpu_queues[min_index].rq1.tail] = cpu_queues[max_index].rq1.processes[cpu_queues[max_index].rq1.tail];
			/* Update queues */
			cpu_queues[min_index].rq1.tail++;
			cpu_queues[max_index].rq1.tail--;
		} else {
			printf("[Balancer] No need to balance this time.\n");
		}
	}




	/* Release the mutex */
	pthread_mutex_unlock(&consumer_mutexes[3]);
	pthread_mutex_unlock(&consumer_mutexes[2]);
	pthread_mutex_unlock(&consumer_mutexes[1]);
	pthread_mutex_unlock(&consumer_mutexes[0]);
}

void clean_up_and_quit(void)
{
	done_flag = 1;
	/* Clean up the mutex */
	if (pthread_mutex_destroy(&mutex) != 0) {
		printf("Failed to release mutex, it's already released.\n");
	}

	/* Clean up consumer mutex */
	int i;
	for(i = 0; i < CORE_NUMBER; ++i)
	{
		if (pthread_mutex_destroy(&consumer_mutexes[i]) != 0) {
			printf("Failed to release mutex, it's already released.\n");
		}
	}


	printf("All done, cleaning up...\n");
    exit(EXIT_SUCCESS);
}

void print_all_queues(void)
{
	int i;
	int core_index;
	
	printf("\n| index |pid|priority|execution time|time_slice|type|\n");
	
	for (core_index = 0; core_index < CORE_NUMBER; ++core_index)
	{
		int rq0_i = 0;
		int rq1_i = 0;
		int rq2_i = 0;
		printf("--------------------Core %d--------------------------\n", core_index);
		// RQ0
		for (i = 0; i < (cpu_queues[core_index].rq0.tail - cpu_queues[core_index].rq0.head); ++i)
		{
			printf("| RQ0_%d | %2d|    %3d |      %4d ms |  %4d ms |", rq0_i++, cpu_queues[core_index].rq0.processes[i].pid, cpu_queues[core_index].rq0.processes[i].priority, cpu_queues[core_index].rq0.processes[i].expected_exec_time, cpu_queues[core_index].rq0.processes[i].time_slice);
			switch(cpu_queues[core_index].rq0.processes[i].schedule_type) {
				case SCHEDULE_FIFO:
					printf("FIFO|\n");
					break;
				case SCHEDULE_RR:
					printf(" RR |\n");
					break;
				case SCHEDULE_NORMAL:
					printf("NORM|\n");
					break;
				default:
					printf("N/A |\n");
			}
		}
		
		// RQ1
		for (i = 0; i < (cpu_queues[core_index].rq1.tail - cpu_queues[core_index].rq1.head); ++i)
		{
			printf("| RQ1_%d | %2d|    %3d |      %4d ms |  %4d ms |", rq1_i++, cpu_queues[core_index].rq1.processes[i].pid, cpu_queues[core_index].rq1.processes[i].priority, cpu_queues[core_index].rq1.processes[i].expected_exec_time, cpu_queues[core_index].rq1.processes[i].time_slice);
			switch(cpu_queues[core_index].rq1.processes[i].schedule_type) {
				case SCHEDULE_FIFO:
					printf("FIFO|\n");
					break;
				case SCHEDULE_RR:
					printf(" RR |\n");
					break;
				case SCHEDULE_NORMAL:
					printf("NORM|\n");
					break;
				default:
					printf("N/A |\n");
			}
		}
		
		// rq2
		for (i = 0; i < (cpu_queues[core_index].rq2.tail - cpu_queues[core_index].rq2.head); ++i)
		{
			printf("| RQ2_%d | %2d|    %3d |      %4d ms |  %4d ms |", rq2_i++, cpu_queues[core_index].rq2.processes[i].pid, cpu_queues[core_index].rq2.processes[i].priority, cpu_queues[core_index].rq2.processes[i].expected_exec_time, cpu_queues[core_index].rq2.processes[i].time_slice);
			switch(cpu_queues[core_index].rq2.processes[i].schedule_type) {
				case SCHEDULE_FIFO:
					printf("FIFO|\n");
					break;
				case SCHEDULE_RR:
					printf(" RR |\n");
					break;
				case SCHEDULE_NORMAL:
					printf("NORM|\n");
					break;
				default:
					printf("N/A |\n");
			}
		}
		
	}
	printf("----------------------------------------------------\n\n");
}
