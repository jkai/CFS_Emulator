/* 
SYSC4001 Assignment3
Student: Junjie Kai	100814819
*/

#include "cpu_queue.h"

/* 
Initialize the multilevel_queue:
	- Set head = 0;
	- Set tail = 0;
*/
void multilevel_queue_init(multilevel_queue* mq)
{
	mq->rq0.head = 0;
	mq->rq1.head = 0;
	mq->rq2.head = 0;
	
	mq->rq0.tail = 0;
	mq->rq1.tail = 0;
	mq->rq2.tail = 0;
}

bool run_queue_empty(run_queue* rq)
{
	return ((rq->tail - rq->head) == 0);
}

bool multilevel_queue_empty(multilevel_queue* mq)
{	
	return (run_queue_empty(&(mq->rq0)) && run_queue_empty(&(mq->rq1)) && run_queue_empty(&(mq->rq2)));	
}

void fetch_queue(run_queue* rq, int core_num)
{
	int queue_count = (rq->tail - rq->head);
	int i;
	int min_index = 0;
	int buffer = rq->processes[0].priority;
	process_struct* process_to_run = NULL;
	
	/* Find the lowest value of priority */
	for (i = 1; i < queue_count; ++i)
	{
		if (rq->processes[i].priority < buffer)
		{
			min_index = i;
			buffer = rq->processes[i].priority;
		}
	}
	
	/* Execute the process */
	process_to_run = &(rq->processes[min_index]);
	printf("[Consumer %d] pid%d is running!\n", core_num, process_to_run->pid);
	run_process(process_to_run);
	printf("[Consumer %d] pid%d is finished!\n", core_num, process_to_run->pid);
	
	/* Remove it */
	for (i = min_index; i < rq->tail; ++i)
	{
		rq->processes[i].pid = rq->processes[i+1].pid;
		rq->processes[i].schedule_type = rq->processes[i+1].schedule_type;
		rq->processes[i].priority = rq->processes[i+1].priority;
		rq->processes[i].expected_exec_time = rq->processes[i+1].expected_exec_time ;
		rq->processes[i].time_slice = rq->processes[i+1].time_slice;
	}
	/* Update Tail */
	rq->tail--;
}

void run_process(process_struct* process_to_run)
{
	int i;
	switch(process_to_run->schedule_type)
	{
		/* FIFO */
		case SCHEDULE_FIFO:
		/* Run it til the end */
		usleep((process_to_run->expected_exec_time)*1000);
		break;
		/* RR */
		case SCHEDULE_RR:
		printf("It's RR");
		
		
		
		break;
		/* NORMAL or Default */
		case SCHEDULE_NORMAL: default:
		printf("It's A NORMAL\n");
		
		break;
	}
}



















