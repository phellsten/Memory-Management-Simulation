//name: Paul Hellsten
//user_id: phellsten

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include 			  <limits.h>

#include        "list.h"
#include        "functions.h"

/* Increment process run time */
void time_tick(List process_list, int time) {
	int i;
	int num = len(process_list);
	for(i=0; i<num;i++) {
		if(((Process*)(process_list->data))->time_created < time) {
			if(((Process*)(process_list->data))->in_memory==0){
				((Process*)(process_list->data))->time_on_disk += 1;
			}
			if(((Process*)(process_list->data))->running == 1){
				((Process*)(process_list->data))->current_run_time += 1;
				((Process*)(process_list->data))->total_running_time += 1;
			}
		}
		process_list = process_list->next;
	}
}

/* Find oldest process that is on disk */
Process* getoldest(List process_list) {
	int i;
	int num = len(process_list);
	int longesttime = 0;
	Process *longestprocess = (Process*)(process_list->data);
	for(i=0; i<num;i++) {
		if(((Process*)(process_list->data))->time_on_disk > longesttime) {
			longestprocess = (Process*)(process_list->data);
			longesttime = ((Process*)(process_list->data))->time_on_disk;
		}
		else if(((Process*)(process_list->data))->time_on_disk >= longesttime) {
			if(((Process*)(process_list->data))->process_id < longestprocess->process_id) {
				longestprocess = (Process*)(process_list->data);
				longesttime = ((Process*)(process_list->data))->time_on_disk;
			}
		}
		process_list = process_list->next;
	}
	return longestprocess;
}

/* Find oldest process that is in memory*/
Process* getoldestinmemory(List process_List) {
	int i;
	int num = len(process_List);
	int longesttime = INT_MAX;
	Process *longestprocess;
	for(i=0; i<num; i++) {
		int timeplaced = ((Process*)(process_List->data))->time_placed_in_memory;
		if(timeplaced != -1) {
			if(timeplaced <= longesttime) {
				longesttime = timeplaced;
				longestprocess = (Process*)(process_List->data);
			}
		}
		process_List = process_List->next;
	}
	return longestprocess;
}

/* Get first free hole that is big enough */
int getfirstfree(List Free_List, int size_to_add) {
	int i;
	int num = len(Free_List);
	int bestend = 0;
	int count = 0;

	for(i=0; i<num; i++) {
		if(((Hole*)(Free_List->data))->size >= size_to_add) {
			bestend = ((Hole*)(Free_List->data))->end_point;
			count++;
		}
		Free_List = Free_List->next;
	}
	if(count == 0) {
		return -1;
	}
	else {
		return bestend - size_to_add + 1;
	}
}

/* Get best size hole relative to size to add */
int getbestfree(List Free_List, int size_to_add) {
	int i;
	int num = len(Free_List);
	int bestend = 0;
	int count = 0;
	int lowestdif = INT_MAX;

	for(i=0; i<num; i++) {
		if(((Hole*)(Free_List->data))->size >= size_to_add) {
			if(((Hole*)(Free_List->data))->size - size_to_add <= lowestdif) {
				lowestdif = ((Hole*)(Free_List->data))->size - size_to_add;
				bestend = ((Hole*)(Free_List->data))->end_point;
				count++;
			}
		}
		Free_List = Free_List->next;
	}
	if(count == 0) {
		return -1;
	}
	else {
		return bestend - size_to_add + 1;
	}
}

/* Get worse sized hole relative to size to add */
int getworstfree(List Free_List, int size_to_add) {
	int i;
	int num = len(Free_List);
	int bestend = 0;
	int count = 0;
	int lowestdif = 0;

	for(i=0; i<num; i++) {
		if(((Hole*)(Free_List->data))->size >= size_to_add) {
			if(((Hole*)(Free_List->data))->size - size_to_add >= lowestdif) {
				lowestdif = ((Hole*)(Free_List->data))->size - size_to_add;
				bestend = ((Hole*)(Free_List->data))->end_point;
				count++;
			}
		}
		Free_List = Free_List->next;
	}
	if(count == 0) {
		return -1;
	}
	else {
		return bestend - size_to_add + 1;
	}
}

/* Set process status to being in memory */
void move_to_memory(List process_list, int id, int memory_loc, int time) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id == id) {
			((Process*)(process_list->data))->location_in_memory = memory_loc;
			((Process*)(process_list->data))->time_placed_in_memory = time;
			((Process*)(process_list->data))->in_memory = 1;
			return;
		}
		process_list = process_list->next;
	}
}

/* Create new hole in the free list */
void free_memory(List freelist, int amount, int location) {
  int i;
	int num = len(freelist);
	for(i=0; i<num; i++) {
    if(location >= ((Hole*)(freelist->data))->end_point) {
      if(num == i+1 && ((Hole*)(freelist->data))->size != 0) {

        List temp_node = malloc(sizeof(List));
        temp_node->data = malloc(sizeof(Hole));

        ((Hole*)(temp_node->data))->start_point = location;
        ((Hole*)(temp_node->data))->end_point = location + amount - 1;
        ((Hole*)(temp_node->data))->size = amount;

        freelist->next = temp_node;

        return;
      }
      if(((Hole*)(freelist->data))->size == 0) {

        ((Hole*)(freelist->data))->start_point = location;
        ((Hole*)(freelist->data))->end_point = location + amount - 1;
        ((Hole*)(freelist->data))->size = amount;
      }

      else {
        freelist = freelist->next;
      }
    }
    else if (location <= ((Hole*)(freelist->data))->end_point) {
      if (location <= ((Hole*)(freelist->data))->start_point) {

        int newstart = (((Hole*)(freelist->data))->start_point);
        int newend = (((Hole*)(freelist->data))->end_point);
        int newsize = (((Hole*)(freelist->data))->size);

        List temp_node = malloc(sizeof(Hole));

        temp_node->data = malloc(sizeof(List));

        ((Hole*)(temp_node->data))->start_point = newstart;
        ((Hole*)(temp_node->data))->end_point = newend;
        ((Hole*)(temp_node->data))->size = newsize;

        temp_node->next = freelist->next;

        ((Hole*)(freelist->data))->start_point = location;
        ((Hole*)(freelist->data))->end_point = location + amount-1;
        ((Hole*)(freelist->data))->size = amount;

        freelist->next = temp_node;

        return;
      }
    }
  }
}

/* Remove hole in the freelist */
void consume_memory(List freelist, int amount, int location) {
  int i;
	int num = len(freelist);

	for(i=0; i<num; i++) {
		int start = ((Hole*)(freelist->data))->start_point;
		int end = ((Hole*)(freelist->data))->end_point;
    if(location >= start && location <= end) {

      List temp_node = malloc(sizeof(Hole));
      temp_node->data = malloc(sizeof(List));
			temp_node->next = malloc(sizeof(List));

      ((Hole*)(temp_node)->data)->start_point = location+amount;
      ((Hole*)(temp_node)->data)->end_point = end;
      ((Hole*)(temp_node)->data)->size = end-location-amount+1;

      temp_node->next = freelist->next;

      freelist->next = temp_node;

      ((Hole*)(freelist->data))->end_point = location-1;
      ((Hole*)(freelist->data))->size = location - start ;

      return;
    }

    freelist = freelist->next;
	}
}

/* Check if sizes in freelist are correct */
void fixsize(List freelist) {
	int i;
	int num = len(freelist);

	for(i=0; i<num; i++) {
		((Hole*)(freelist->data))->size = ((Hole*)(freelist->data))->end_point - ((Hole*)(freelist->data))->start_point + 1;
		freelist = freelist->next;
	}
}

/* If there are any holes that should be merged into the same hole */
List checkholes(List freelist, int lastpos) {
	List new_freelist = malloc(sizeof(Hole));
	int count = 0;

	while(freelist->next != NULL) {
		if(((Hole*)(freelist->data))->end_point == lastpos) {
			freelist->next = NULL;
		}
		else if (((Hole*)(freelist->data))->end_point == ((Hole*)(freelist->next->data))->start_point - 1) {
			((Hole*)(freelist->data))->end_point = ((Hole*)(freelist->next->data))->end_point;
			((Hole*)(freelist->data))->size += ((Hole*)(freelist->next->data))->size;
			freelist->next = freelist->next->next;
		}
		else {
			if(count == 0) {
				new_freelist = push(NULL, freelist->data);
			}
			else {
				insert(freelist->data, &new_freelist);
			}
			count++;

			freelist = freelist->next;
		}
	}
	if(count == 0) {
		new_freelist = push(NULL, freelist->data);
	}
	else {
		insert(freelist->data, &new_freelist);
	}
	fixsize(new_freelist);
	return new_freelist;
}

/* Check if there are any holes of size 0, and remove them */
List checkifempty(List freelist) {
  int i;
  int num = len(freelist);

	List new_freelist = malloc(sizeof(Hole));
  int size = 0;

  for(i=0; i<num; i++) {
    if(((Hole*)(freelist)->data)->size != 0) {
      if(size == 0) {
        new_freelist = push(NULL, (Hole*)(freelist)->data);
        size++;
      }
      else {
        insert((Hole*)(freelist)->data, &new_freelist);
      }
    }

    freelist = freelist->next;
  }

  if(size == 0) {
     Hole *temphole = (Hole*)malloc(sizeof(Hole));
     temphole->size = 0;
     temphole->start_point = 0;
     temphole->end_point = 0;
     new_freelist = push(NULL, temphole);
   }
  return new_freelist;
}

/* Remove process from cpu queue */
List remove_from_cpu_queue(List cpu_queue, int id) {
	int i;
	int num = len(cpu_queue);
	List new_cpu_queue = malloc(sizeof(Process));
	int len = 0;

	for(i=0; i<num; i++) {
		if(((Process*)(cpu_queue->data))->process_id != id) {
			if(len == 0) {
				new_cpu_queue = push(NULL, cpu_queue->data);
			}
			else {
				insert(cpu_queue->data, &new_cpu_queue);
			}
			len++;
		}
		cpu_queue = cpu_queue->next;
	}
	return new_cpu_queue;
}

/* Set process as not in memory */
void remove_from_memory(List process_list, int id) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id == id) {
			((Process*)(process_list->data))->running = 0;
			((Process*)(process_list->data))->current_run_time = 0;
			((Process*)(process_list->data))->in_memory = 0;
			((Process*)(process_list->data))->time_on_disk = 0;
			((Process*)(process_list->data))->time_placed_in_memory = -1;
			((Process*)(process_list->data))->location_in_memory = -1;
			return;
		}
		process_list = process_list->next;
	}
}

/* Set process as not currently running */
void stoprunning(int id, List process_list) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id == id) {
			((Process*)(process_list->data))->running = 0;
			((Process*)(process_list->data))->current_run_time = 0;
			return;
		}
		process_list = process_list->next;
	}
}

/* Set process as currently running */
void startrunning(int id, List process_list) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id == id) {
			((Process*)(process_list->data))->running = 1;
			return;
		}
		process_list = process_list->next;
	}
}

/* Get the run time of a process */
int getcurrentruntime(int id, List process_list) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id == id) {
			return ((Process*)(process_list->data))->current_run_time;
		}
		process_list = process_list->next;
	}
	return 0;
}

/* Get the process id of a process */
Process* getprocess(int id, List process_list) {
	int i;
	int num = len(process_list);
	Process *processtoget;
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id == id) {

			processtoget = (Process*)(process_list->data);
		}
		process_list = process_list->next;
	}
	return processtoget;
}

/* Check if process has reached it's total job time */
int checkifdone(int processid, List process_list) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id ==processid) {
			if(((Process*)(process_list->data))->total_running_time == ((Process*)(process_list->data))->job_time) {
				((Process*)(process_list->data))->done = 1;
				return 1;
			}
			else {
				return 0;
			}
		}
		process_list = process_list->next;
	}
	return 0;
}

/* Get list of processes waiting on disk */
List process_waiting(List processes, int time) {
	List processes_waiting = malloc(sizeof(Process));
	List process = malloc(sizeof(Process));
	process = processes;
	int i;
	int length = len(process);
	int num = 0;
	for(i=0; i<length; i++) {
		if(((Process*)(process->data))->time_created <= time
				&& ((Process*)(process->data))->in_memory == 0
					&& ((Process*)(process->data))->done == 0) {

			if(num==0) {
				processes_waiting = push(NULL, process->data);
				num++;
			}
			else {
				processes_waiting = push(processes_waiting, process->data);
				num++;
			}
		}
		process=process->next;
	}
	if(num == 0) {
		return NULL;
	}
	else {
		return processes_waiting;
	}
}

/* Check if process is in memory or not */
int in_memory(List process_list, int id) {
	int i;
	int num = len(process_list);
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->process_id ==id) {
			if(((Process*)(process_list->data))->in_memory == 0) {
				return 0;
			}
			else {
				return 1;
			}
		}
		process_list = process_list->next;
	}
	return 0;
}

/* Get sum of all holes */
int memory_usage(List freelist) {
	int i;
	int num = len(freelist);
	int sum = 0;
	for(i=0;i<num;i++) {
		sum += ((Hole*)(freelist->data))->size;
		freelist = freelist->next;
	}

	return sum;
}

/* Get number of processes that are in memory */
int num_in_memory(List process_list) {
	int i;
	int num = len(process_list);
	int sum = 0;
	for(i=0; i<num; i++) {
		if(((Process*)(process_list->data))->in_memory) {
			sum++;
		}
		process_list = process_list->next;
	}
	return sum;
}
