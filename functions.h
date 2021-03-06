//name: Paul Hellsten
//user_id: phellsten

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "list.h"

typedef struct process_t {
	int time_created;
	int process_id;
	int memory_size;
	int job_time;
	int current_run_time;
	int total_running_time;
	int time_on_disk;
	int done;
	int in_memory;
	int running;
	int time_placed_in_memory;
	int location_in_memory;
}Process;

typedef struct free_list_t {
	int start_point;
	int end_point;
	int size;
}Hole;

void fixsize(List freelist);
int num_in_memory(List process_list);
int memory_usage(List freelist);
List remove_from_cpu_queue(List cpu_queue, int id);
void remove_from_memory(List process_list, int id);
Process* getoldestinmemory(List process_List);
List checkholes(List freelist, int lastpos);
List checkifempty(List freelist);
void free_memory(List freelist, int amount, int location);
int checkifdone(int processid, List process_list);
Process* getprocess(int currently_running, List process_list);
int getcurrentruntime(int id, List process_list);
void startrunning(int id, List process_list);
void stoprunning(int id, List process_list);
void consume_memory(List freelist, int amount, int location);
void move_to_memory(List process_list, int id, int memory_loc, int time);
int getfirstfree(List Free_List, int size_to_add);
int getbestfree(List Free_List, int size_to_add);
int getworstfree(List Free_List, int size_to_add);
Process* getoldest(List process_list);
void time_tick(List process_list, int time);
void printfreelist(List freelist);
void print_listt(List process_list);
void print_processids(List process_list);
List process_waiting(List processes, int time);

#endif
