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
			rq->processes[i] = rq->processes[i+1];
		}
		/* Update Tail */
		rq->tail--;
	}
	
}

void run_process(process_struct* process_to_run, int core_num)
{
	int i;
	int service_time;
	struct timeval current_time;

	switch(process_to_run->schedule_type)
	{
		/* FIFO */
		case SCHEDULE_FIFO:
		/* Run it til the end */
		printf("[Consumer %d][pid%d][FIFO] with %d priority is selected to run for %dms.\n", core_num, process_to_run->pid, process_to_run->priority, process_to_run->expected_exec_time);
		usleep((process_to_run->expected_exec_time)*1000);
		process_to_run->finished = 1;
		gettimeofday(&current_time, NULL);
		printf("[Consumer %d][pid%d][FIFO] is finished, the turnaround time is %dms.\n", core_num, process_to_run->pid, get_tat(process_to_run->started_time, current_time));
		break;
		
		/* RR */
		case SCHEDULE_RR:
		/* Dynamic time slice */
		printf("[Consumer %d][pid%d][RR] with %d priority is selected, remaining exec time is %dms.\n", core_num, process_to_run->pid, process_to_run->priority, process_to_run->expected_exec_time);
		//Get Service time, either 10 ms or time_slice
		service_time = 10 + ((process_to_run->time_slice - 10)*(rand() % 2));
		if (service_time >= process_to_run->expected_exec_time)
		{
			//Finish the task
			printf("[Consumer %d][pid%d][RR] is selected to run for %dms.\n", core_num, process_to_run->pid, process_to_run->expected_exec_time);
			usleep((process_to_run->expected_exec_time)*1000);
			process_to_run->expected_exec_time = 0;
			process_to_run->finished = 1;
			gettimeofday(&current_time, NULL);
			printf("[Consumer %d][pid%d][RR] is finished, the turnaround time is %dms.\n", core_num, process_to_run->pid, get_tat(process_to_run->started_time, current_time));
		} else {
			if (service_time == process_to_run->time_slice) {
				printf("[Consumer %d][pid%d][RR] is selected to run %dms.\n", core_num, process_to_run->pid, service_time);
				usleep(service_time * 1000);
			} else {
				printf("[Consumer %d][pid%d][RR] is blocked, time slice allocated = %dms, time slice used = %dms.\n", core_num, process_to_run->pid, process_to_run->time_slice,service_time);
				usleep((service_time * 1000) + WAIT_TIME *1000);
			}
			process_to_run->expected_exec_time -= service_time;
		}
		break;
		
		/* NORMAL or Default */
		case SCHEDULE_NORMAL: default:

		if (process_to_run->time_slice >= process_to_run->expected_exec_time)
		{
			//Finish the task
			printf("[Consumer %d][pid%d][NORMAL] with %d priority is selected to run for %dms, remaining exec time is %dms.\n", core_num, process_to_run->pid, process_to_run->priority, process_to_run->expected_exec_time, process_to_run->expected_exec_time);
			usleep(process_to_run->expected_exec_time * 1000);
			gettimeofday(&current_time, NULL);
			printf("[Consumer %d][pid%d][NORMAL] is finished, the turnaround time is %dms.\n", core_num, process_to_run->pid, get_tat(process_to_run->started_time, current_time));
			process_to_run->expected_exec_time = 0;
			process_to_run->finished = 1;
		} else {
			// Increment sleep_avg
			printf("[Consumer %d][pid%d][NORMAL] with %d priority is selected to run for %dms, remaining exec time is %dms.\n", core_num, process_to_run->pid, process_to_run->priority, process_to_run->time_slice, process_to_run->expected_exec_time);
			gettimeofday(&current_time, NULL);
	        int ticks = calculate_ticks(process_to_run->last_sleep_time, current_time);
	        process_to_run->sleep_avg = min(process_to_run->sleep_avg + ticks, MAX_SLEEP_AVG);

	        //Update priority
	        process_to_run->priority = calculate_dp(process_to_run->priority, process_to_run->sleep_avg);
	        printf("[Consumer %d][pid%d][NORMAL]'s priority has been updated to %d.\n", core_num, process_to_run->pid, process_to_run->priority);

	        //Run
	        struct timeval t1, t2;
	        gettimeofday(&t1, NULL);
	        usleep(process_to_run->time_slice * 1000);
	        gettimeofday(&t2, NULL);

	        // Decrement sleep_avg
	        ticks = calculate_ticks(t1, t2);
	        process_to_run->sleep_avg = max(process_to_run->sleep_avg - ticks, 0);

	        // Update
	        process_to_run->expected_exec_time -= process_to_run->time_slice;
	        gettimeofday(&process_to_run->last_sleep_time, NULL);
		}

		break;
	}
}

//Turnaround time
int get_tat (struct timeval t1, struct timeval t2)
{
	int time1 = (t1.tv_sec*1000) + (t1.tv_usec/1000);
    int time2 = (t2.tv_sec*1000) + (t2.tv_usec/1000);
    return (time2-time1);
}

int calculate_time_slice(int sp) {
    if (sp < 120) return (140 - sp) * 20;
    else return (140 - sp) * 5;
}

int calculate_ticks(struct timeval t1, struct timeval t2) {
    int time1 = (t1.tv_sec*1000) + (t1.tv_usec/1000);
    int time2 = (t2.tv_sec*1000) + (t2.tv_usec/1000);
    return (time2-time1) / 100;
}

int calculate_dp(int previous_dp, int bonus) {
    return max(100, min(previous_dp - bonus + 5, 139));
}

int min(int a, int b) {return (a<=b ? a:b);}
int max(int a, int b) {return (a>b ? a:b);}


















