/* 
SYSC4001 Assignment3
Student: Junjie Kai	100814819
*/

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Common.h"
#include "cpu_queue.h"

/* 
Initialize the multilevel_queue:
	- Set head = 0;
	- Set tail = 0;
*/
void multilevel_queue_init(multilevel_queue* mq)
{
	mg->rq0.head = 0;
	mg->rq1.head = 0;
	mg->rq2.head = 0;
	
	mg->rq0.tail = 0;
	mg->rq1.tail = 0;
	mg->rq2.tail = 0;
}
