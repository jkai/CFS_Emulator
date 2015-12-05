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
#include <time.h>
#include <sys/time.h>

#include "Common.h"

void multilevel_queue_init(multilevel_queue* mq);
bool run_queue_empty (run_queue* rq);
bool multilevel_queue_empty(multilevel_queue* mq);
void fetch_queue(run_queue* rq, int core_num);
int get_tat (struct timeval t1, struct timeval t2);
void run_process(process_struct* process_to_run, int core_num);
int calculate_time_slice(int sp);
int calculate_ticks(struct timeval t1, struct timeval t2);
int calculate_dp(int previous_dp, int bonus);
int min(int a, int b);
int max(int a, int b);

#endif
