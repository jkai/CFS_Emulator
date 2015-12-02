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
