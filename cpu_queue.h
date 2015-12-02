/* 
SYSC4001 Assignment3
Student: Junjie Kai	100814819
*/

#ifndef Assignment3_cpu_queue_h
#define Assignment3_cpu_queue_h

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Common.h"

void multilevel_queue_init(multilevel_queue* mq);
bool run_queue_empty (run_queue* rq);
bool multilevel_queue_empty(multilevel_queue* mq);
void fetch_queue(run_queue* rq, int core_num);
void run_process(process_struct* process_to_run);

#endif
