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
	run_process(process_to_run, core_num);
	
	/* Remove it if finished*/
	if (process_to_run->finished)
	{
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
	
}

void run_process(process_struct* process_to_run, int core_num)
{
	int i;
	int service_time;
	switch(process_to_run->schedule_type)
	{
		/* FIFO */
		case SCHEDULE_FIFO:
		/* Run it til the end */
		printf("[Consumer %d] pid%d[FIFO] with %d priority is selected to run for %ms.\n", core_num, process_to_run->pid, process_to_run->priority, process_to_run->expected_exec_time);
		usleep((process_to_run->expected_exec_time)*1000);
		process_to_run->finished = 1;
		printf("[Consumer %d] pid%d[FIFO] is finished, the turnaround time is %ms.\n", core_num, process_to_run->pid, process_to_run->expected_exec_time);
		break;
		
		/* RR */
		case SCHEDULE_RR:
		/* Dynamic time slice */
		
		//Get Service time, either 100 ms or time_slice
		service_time = 100 + ((process_to_run->time_slice - 100)*(rand() % 2));
		if (service_time >= process_to_run->expected_exec_time)
		{
			//Finish the task
			printf("[Consumer %d] pid%d[RR] with %d priority is selected to run for %ms.\n", core_num, process_to_run->pid, process_to_run->priority, process_to_run->expected_exec_time);
			usleep((process_to_run->expected_exec_time)*1000);
			process_to_run->expected_exec_time = 0;
			process_to_run->finished = 1;
			printf("[Consumer %d] pid%d[RR] is finished, its turnaround time is \n", core_num, process_to_run->pid);
		} else {
			if (service_time == process_to_run->time_slice) {
				printf("[Consumer %d] pid%d_RR is running for %d ms, no I/O!\n", core_num, process_to_run->pid, service_time);
				usleep(service_time * 1000);
			} else {
				printf("[Consumer %d] pid%d_RR is running for %d ms, due to I/O!\n", core_num, process_to_run->pid, service_time + WAIT_TIME);
				usleep((service_time * 1000) + WAIT_TIME *1000);
			}
			process_to_run->expected_exec_time -= service_time;
		}
		break;
		
		/* NORMAL or Default */
		case SCHEDULE_NORMAL: default:
		printf("[Consumer %d] pid%d_NORMAL is running for %d ms!\n", core_num, process_to_run->pid, process_to_run->expected_exec_time);
		process_to_run->finished = 1;
		break;
	}
}



















