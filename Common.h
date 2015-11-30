/* 
SYSC4001 Assignment3
Student: Junjie Kai	100814819
*/

#ifndef Assignment3_common_h
#define Assignment3_common_h

/* 4-core with 20 initial processes */
#define CORE_NUMBER	4
#define DEFAULT_PROCESS_NUMBER	20

/* Process schedule types: FIFO, RR, NORMAL */
#define SCHEDULE_FIFO 0
#define SCHEDULE_RR 1
#define SCHEDULE_NORMAL 2

/* Each run queue has 20 slots */
#define RUN_QUEUE_SIZE 20

typedef struct {
	int pid;									//Generated Process ID, starts from 1
	int schedule_type;							//Schedule types: FIFO, RR, NORMAL
	int static_priority;						//Static priority
	int dynamic_priority;						//Dynamic priority
	int expected_exec_time;						//Expected Execution time
	int time_slice;								//Time slice
	int accu_time_slice;						//Accumulated time slice
} process_struct;

/* Single run queue, acts as a circular buffer */
typedef struct {
	int head;									//Head of circular buffer
	int tail;									//Tail of circular buffer
	int count;									//Count of items in circular buffer
	process_struct processes[RUN_QUEUE_SIZE];	//Max 20 processes in a run_queue
} run_queue;

/* 
Multilevel queues for each CPU:
	====================================================
	RQ0:	* SCHED_FIFO and SCHED_RR
			* priority range (0, 100)
	====================================================
	RQ1:	* SCHED_NORMAL
			* priority range (100, 130)
	====================================================
	RQ2:	* SCHED_NORMAL
			* priority range (130, ~)
	====================================================
 */
typedef struct {
	run_queue rq0;
	run_queue rq1;
	run_queue rq2;
} multilevel_queue;

#endif
