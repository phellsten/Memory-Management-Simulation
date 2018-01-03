//name: Paul Hellsten
//user_id: phellsten

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <getopt.h>
#include        <stdbool.h>
#include        <math.h>
#include        "list.h"
#include        "functions.h"

#define LEN 1000

/* Extern declarations: */
extern  int     optind;
extern  char    *optarg;

int main(int argc, char** argv) {

	/* Default parameters */
	char input;
  char *filename = "input.txt";
	char *algorithm = "first";
	int memsize = 1000;
  int quantum = 7;

  /* Parse command line arguments*/
  while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF) {
    switch ( input )
  	{
      case 'f':
        filename = optarg;
        break;
  		case 'a':
  			if(strcmp(optarg, "worst") == 0) {
  				algorithm  = optarg;
				}
  			else if(strcmp(optarg, "best") == 0) {
  				algorithm  = optarg;
				}
  			else if(strcmp(optarg, "first") != 0) {
  				fprintf(stderr, "Unknown %s\n ", optarg);
  				exit(1);
  			}
   			break;
			case 'm':
				memsize = atoi(optarg);
				break;
      case 'q':
        quantum = atoi(optarg);
        break;
		}
	}

  /* Create free list with a single hole of size memsize */
	Hole *hole = (Hole*)malloc(sizeof(Hole));
	hole->start_point = 1;
	hole->end_point = memsize;
	hole->size = memsize;
	List Free_List = push(NULL, hole);

	int c, num = 0, numfinished = 0;
	bool new_line = true;
	List process_list;
	FILE *file;
	file = fopen(filename, "r");

  /* Parse input from file */
	if (file) {
		char inarray[LEN];
		int i = 0;
		while ((c = getc(file))) {

      /* If reached end of the line, stop reading in characters */
			if (c == '\n' || c == EOF) {
				if(new_line) {
					break;
				}

				inarray[i] = '\0';
				char *ch;
        char* process_data[5];
				int j = 0;

        /* Split up line into character array of each seperate number */
				ch = strtok(inarray, " ");
				while (ch != NULL) {
					process_data[j] = malloc(sizeof(ch));
					strcpy(process_data[j], ch);
					j++;
					ch = strtok(NULL, " ,");
				}

        /* Initialise process */
				Process *new_process = (Process*)malloc(sizeof(Process));

				new_process->time_created = atoi(process_data[0]);
				new_process->process_id = atoi(process_data[1]);
				new_process->memory_size = atoi(process_data[2]);
				new_process->job_time = atoi(process_data[3]);
				new_process->current_run_time = 0;
				new_process->total_running_time = 0;
				new_process->time_on_disk = 0;
				new_process->done = 0;
				new_process->in_memory = 0;
				new_process->running = 0;
				new_process->time_placed_in_memory = -1;
				new_process->location_in_memory = -1;

        /* Add process to process list */
				if (num == 0) {
					process_list = push(NULL, new_process);
				}
				else {
					process_list = push(process_list, new_process);
				}

				num++;
				i = 0;
				j = 0;
				new_line = true;

				if(c == EOF) {
					break;
				}
			}
      /* Parse new character into array */
			else {
				new_line = false;
				inarray[i] = c;
				i++;
			}
		}
		fclose(file);
	}

  /* Store processes in order of input */
	process_list = reverse(process_list);

	int currently_running, time = 0, processes_in_queue = 0;
	bool process_running = false, timetoswap = false, logline = false;
	List cpu_queue = malloc(sizeof(List));

  /* Each time through the loop is one time period of the simulaltion */
	while(1) {
		List processes_waiting = malloc(sizeof(List));
		processes_waiting = process_waiting(process_list, time);

    /* Check if process has completed it's quantum. */
		if(process_running) {
			int timerunning = getcurrentruntime(currently_running, process_list);
			if(timerunning == quantum) {
				timetoswap = true;
			}
		}

    /* Event E1 - Process has been created and memory is currently empty */
		if(processes_waiting != NULL && ((Hole*)(Free_List->data))->size == memsize) {
		}
    /* Event E2 - Quantum has expired for process running on the CPU  */
		else if(timetoswap) {
		}
    /* Event E3 - Process running on the CPU has finished */
		else if(checkifdone(currently_running, process_list)) {
		}
    /* If no events have occured, increment time and return to the loop */
		else {
			time++;
			time_tick(process_list, time);
			continue;
		}
		timetoswap = false;
    /*
      Check if process has finished,
      If it has, stop running and remove it from memory
    */
		if (checkifdone(currently_running, process_list)) {
			free_memory(Free_List, getprocess(currently_running, process_list)->memory_size, getprocess(currently_running, process_list)->location_in_memory);
			remove_from_memory(process_list, currently_running);
			Free_List = checkholes(Free_List, memsize);
			numfinished++;
			stoprunning(currently_running, process_list);
			currently_running = -1;
			process_running = false;
		}
    /* If there are processes on disk, swap one to memory*/
		if(processes_waiting != NULL) {
			Process *process_to_add = getoldest(processes_waiting);
			int size_to_add = process_to_add->memory_size;
			int memory_loc;

      /* Get an appropriately sized hole based on input algorithm */
			if(strcmp(algorithm,"first") == 0) {
				memory_loc = getfirstfree(Free_List, size_to_add);
			}
			else if(strcmp(algorithm,"best") == 0) {
				memory_loc = getbestfree(Free_List, size_to_add);
			}
			else if(strcmp(algorithm,"worst") == 0) {
				memory_loc = getworstfree(Free_List, size_to_add);
			}

      /* If process cannot fit in memory, keep removing oldest process */
			while(memory_loc == -1) {
				if(strcmp(algorithm,"first") == 0) {
					memory_loc = getfirstfree(Free_List, size_to_add);
				}
				else if(strcmp(algorithm,"best") == 0) {
					memory_loc = getbestfree(Free_List, size_to_add);
				}
				else if(strcmp(algorithm,"worst") == 0) {
					memory_loc = getworstfree(Free_List, size_to_add);
				}

        /* Process can fit */
				if(memory_loc != -1) {
					break;
				}

				Process *process_to_remove = getoldestinmemory(process_list);

        /*
           If oldest process is currently running, check if it's finished and
           stop it running
        */
				if(currently_running == process_to_remove->process_id) {
					if (checkifdone(currently_running, process_list)) {
						remove_from_memory(process_list, process_to_remove->process_id);
						numfinished++;
						stoprunning(currently_running, process_list);
						currently_running = -1;
						process_running = false;
					}
					else {
						process_running = false;
						processes_in_queue++;
					}
				}

        /* Free memory assigned to oldest process, and set it as not in memory */
				free_memory(Free_List, process_to_remove->memory_size, process_to_remove->location_in_memory);
				remove_from_memory(process_list, process_to_remove->process_id);
				cpu_queue = remove_from_cpu_queue(cpu_queue, process_to_remove->process_id);
				Free_List = checkholes(Free_List, memsize);
				processes_in_queue --;
			}

      /* Move new proecss into memory and into the cpu queue */
			move_to_memory(process_list, process_to_add->process_id, memory_loc, time);
			consume_memory(Free_List, process_to_add->memory_size, memory_loc);
			Free_List = checkifempty(Free_List);
			if(processes_in_queue == 0) {
				cpu_queue = push(NULL, process_to_add);
			}
			else {
				insert(process_to_add, &cpu_queue);
			}

			processes_in_queue++;
			logline = true;

			printf("time %d, %d loaded, ", time, process_to_add->process_id);
		}

    /*
       Check if currently running process has finished.
       If it hasn't finished, add it back into the cpu queue.
    */
		if(!checkifdone(currently_running, process_list)) {
			if(process_running == true) {
				stoprunning(currently_running, process_list);

				if(processes_in_queue == 0) {
					cpu_queue = push(NULL, getprocess(currently_running, process_list));
				}
				else {
					insert(getprocess(currently_running, process_list), &cpu_queue);
				}
				processes_in_queue ++;
			}
		}
		else if(checkifdone(currently_running, process_list)){
			numfinished++;
			stoprunning(currently_running, process_list);
			free_memory(Free_List, getprocess(currently_running, process_list)->memory_size, getprocess(currently_running, process_list)->location_in_memory);
			remove_from_memory(process_list, currently_running);
			Free_List =checkholes(Free_List, memsize);
			currently_running = -1;
		}

		/* Check if all processes have finished running, and print final line. */
		if(numfinished == len(process_list)) {
			printf("time %d, simulation finished.\n", time);

			exit(EXIT_SUCCESS);
		}
		else {
      /*
         If no processes are in the queue, we must be waiting for a process
         that hasn't been created yet. CPU will be idle.
      */
			if(len(cpu_queue) == 0 ) {
				continue;
			}
		}

    /* Pop the next process from the cpu queue and start running it. */
		Process *next_process = (Process*)malloc(sizeof(Process));
		next_process = (Process*)pop(&cpu_queue);
		processes_in_queue --;
		startrunning(next_process->process_id, process_list);
		process_running = true;
		currently_running = next_process->process_id;

		if(logline) {
			logline = false;

			/* Number of processes currently in memory */
			int numinmemory = num_in_memory(process_list);

			/* Gets total size of the free list. */
			int memfree = memory_usage(Free_List);

			/* Gets current memory usage */
			int memusage = (int)(memsize - memfree);

			/* Convert memory usage to a percentage of total memory */
			double memusage2 = (1.0*memusage / memsize) * 100;
			memusage = ceil(memusage2);

			int number_holes = len(Free_List);

			if(memfree == 0) {
				number_holes = 0;
			}

			printf("numprocesses=%d, numholes=%d, memusage=%d%%\n", numinmemory, number_holes, (int)memusage);

		}

		time++;
		time_tick(process_list, time);

	}

  return 0;
}
