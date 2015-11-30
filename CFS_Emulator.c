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
void initial_cpu_queues(void);
void wait_for_producer(void);
void print_queue (int core_num);
void print_process_info (process_struct *process);
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
	
	/* Seed the random number generater with pid */
	srand(getpid());
	
	/* Generate producer thread and consumer threads*/
    generate_producer();
	/* Wait for producer to produce */
	wait_for_producer();

	print_queue(1);
	
	
	
    //generate_consumers();
	

	
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
    printf("[Producer] Creation successful! Generating processes...\n");
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
	int i;			//Index
	int core_num;		//Define which core to assign the generated processes
	process_struct *current_process	= NULL;	//Point to the process in queue
	
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
			
				//Update count
				cpu_queues[core_num].rq1.count++;
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
				//Done
				printf("[Producer] A Normal process is created for core%d:\n", core_num);
				print_process_info (current_process);
				break;

			/* RR */
			case 3:
			/* Update the RQ0 queue */
			
				//Update count
				cpu_queues[core_num].rq0.count++;
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
				current_process->time_slice = DEFAULT_TIME_SLICE;
				//Done
				printf("[Producer] A RR process is created for core%d:\n", core_num);
				print_process_info (current_process);
			break;
			/* FIFO */
			case 4:
			/* Update the RQ0 queue */
			
				//Update count
				cpu_queues[core_num].rq0.count++;
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
				//Done
				printf("[Producer] A FIFO process is created for core%d:\n", core_num);
				print_process_info (current_process);
			break;
			default:
				fprintf(stderr, "Invalid process type, quitting...\n");
				exit(EXIT_FAILURE);	
		}
		
		
	}

}

void initial_cpu_queues(void)
{
	int i;
	printf("[Producer] Initializing cpu queues...\n");
	
	/* 
	Initialize the cpu_queues:
		- Set head = 0
		- Set tail = 0
		- Set count = 0;
	 */
	 for (i = 0; i < CORE_NUMBER; ++i)
	 {
		 //Set heads
		 cpu_queues[i].rq0.head = 0;
		 cpu_queues[i].rq1.head = 0;
		 cpu_queues[i].rq2.head = 0;
		 //Set Tails
		 cpu_queues[i].rq0.tail = 0;
		 cpu_queues[i].rq1.tail = 0;
		 cpu_queues[i].rq2.tail = 0;
		 //Set counts
		 cpu_queues[i].rq0.count = 0;
		 cpu_queues[i].rq1.count = 0;
		 cpu_queues[i].rq2.count = 0;
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

void print_queue (int core_num)
{
	int i;
	printf("CPU[%d]'s run queues:\n", core_num);
	printf("-----------------------------------------\nRQ0:\n");
	for (i = 0; i < cpu_queues[core_num].rq0.count; ++i)
	{
		printf("pid%02d|", cpu_queues[core_num].rq0.processes[i].pid);
	}
	printf("\n-----------------------------------------\nRQ1:\n");
	for (i = 0; i < cpu_queues[core_num].rq1.count; ++i)
	{
		printf("pid%02d|", cpu_queues[core_num].rq1.processes[i].pid);
	}
	printf("\n-----------------------------------------\nRQ2:\n");
	for (i = 0; i < cpu_queues[core_num].rq2.count; ++i)
	{
		printf("pid%02d|", cpu_queues[core_num].rq2.processes[i].pid);
	}
	printf("\n-----------------------------------------\n");
}

void print_process_info (process_struct *process)
{
	printf("| [pid] = %03d |", process->pid);
	printf(" [priority] = %03d |", process->priority);
	printf(" [exec time] = %04d ms |", process->expected_exec_time);

	switch(process->schedule_type) {
		case SCHEDULE_FIFO:
			printf("   FIFO   |\n");
			break;
		case SCHEDULE_RR:
			printf("    RR    |\n");
			break;
		case SCHEDULE_NORMAL:
			printf("  NORMAL  |\n");
			break;
		default:
			printf("  N/A   |\n");
	}
}

void clean_up_and_quit(void)
{
	printf("All done, cleaning up...\n");
    exit(EXIT_SUCCESS);
}
