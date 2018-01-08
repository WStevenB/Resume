/* xsh_date.c - xsh_date */

#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "process_ring.h"


shellcmd xsh_process_ring(int nargs, char *args[]) {
	//loop counter
	int i = 0;
	
	//default settings
	int32 process_count = 2;
	int32 number_rounds = 3;
	int32 mode = 0;                 //0 is poll and 1 is sync
	
	//parse input
	//help arg
	if (nargs == 2 && 0 == strncmp("--help", args[1], 6)){
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tCreates several processes which communicate to countdown a number to zero\n");
		printf("\t-p lets you specify the number of processes from 0 to 64\n");
		printf("\t-r lets you specify the number of rounds from 1 to 10000\n");
		printf("\t-i lets you specify between either poll or sync (methods the processes use to communicate)\n");
		return 0;
	}
	else {
		i = 1;
		while(i < nargs-1){
			
			//process arg
			if (0 == strncmp("-p", args[i], 2)){
				int num = atoi(args[i + 1]);
            	if (!((num != 0 || strncmp(args[i+1], "0", 1) == 0) && 0 <= num && num <= 64)){
               		printf("-p expected numeric argument between 0-64\n");
               		return SHELL_ERROR;
            	}
            	else process_count = num;
            	i += 1;
			}
			//round arg
			else if (0 == strncmp("-r", args[i], 2)){
				int num = atoi(args[i + 1]);
            	if (!((num != 0 || strncmp(args[i+1], "0", 1) == 0) && 0 <= num && num <= 10000)){
               		printf("-r expected numeric argument between 0-10000\n");
               		return SHELL_ERROR;
            	}
            	else number_rounds = num;
            	i += 1;
			}
			//polling or semaphore arg
			else if (0 == strncmp("-i", args[i], 2)){
				if (0 == strncmp("sync", args[i+1], 4)) mode = 1;
			}
			i++;
		}
	}
	
	//start printing output
	printf("Number of Processes: %d\n", process_count);
	printf("Number of Rounds: %d\n", number_rounds);
	
	//either processes or rounds was set to zero... just print output with no countdown
	if(process_count == 0 || number_rounds == 0) {
		printf("Elapsed Seconds: 0\n");
		return SHELL_OK;
	}
	
	//calculate number to countdown
	int countdown = process_count * number_rounds - 1;
	
	//record start time
	uint32 now;
	gettime(&now);
	
	//create processes and start countdown
	//polling
	if(mode == 0) {
		//flag that processes will use to coordinate exiting
		int shouldWait = 0;
		
		//get current process id
		pid32 parent_pid  = getpid();
		
		//create ring processes, put pids in an array, and start them
		pid32 ring_ids [process_count];
		i = 0;
		while(i<process_count) {
			ring_ids[i] = create(polling_ring_process, 1024, 20, "ring_member", 4, parent_pid, number_rounds, (int32)(i), (int32) (&shouldWait));
			i++;
		}
		i = 0;
		while(i<process_count) {
			resume(ring_ids[i]);
			i++;
		}
		
		//send each process a message containing the appropriate pid of the next process in the ring
		i = 0;
		while(i<process_count-1) {
			send(ring_ids[i], ring_ids[i+1]);
			i++;
		}
		send(ring_ids[process_count-1], ring_ids[0]);
		
		//start the countdown
		send(ring_ids[0], countdown);
		
		//receive the exit code for each child
		//set wait back to 0, each child will set it to 1 just before sending exit notice
		//other children will wait until the flag is reset before exiting
		i = 0;
		while(i<process_count) {
			receive();
			shouldWait  = 0;
			i++;
		}
		//print time taken
		uint32 later;
		gettime(&later);
		printf("Elapsed Seconds: %d\n", later-now+1);
    	return SHELL_OK;
	}
	//sync
	else {
		//flag that processes will use to coordinate exiting
		int shouldWait = 0;
		
		//create and place ring semaphores' sids in array
		sid32 sem_ids [process_count];
		i = 0;
		while(i<process_count) {
			sem_ids[i] = semcreate(0);
			i++;
		}
		//create finishing semaphore
		sid32 done_sem = semcreate(0);
		
		//create a process for each semaphore/member of ring
		i = 0;
		while(i<process_count-1) {
			resume(create(semaphore_ring_process, 1024, 20, "ring_member", 7, sem_ids[i], sem_ids[i+1], done_sem, number_rounds, i, (int32) (&countdown), (int32) (&shouldWait)));
			i++;
		}
		resume(create(semaphore_ring_process, 1024, 20, "ring_member", 7, sem_ids[process_count-1], sem_ids[0], done_sem, number_rounds, process_count-1, (int32) (&countdown), (int32) (&shouldWait)));
		
		//start the countdown
		signal(sem_ids[0]);
		
		//wait for each child to finish
		//wait flag works similar to above
		i = 0;
		while(i<process_count) {
			wait(done_sem);
			shouldWait = 0;
			i++;
		}
		
		//delete all semaphores
		i = 0;
		while(i<process_count) {
			semdelete(sem_ids[i]);
			i++;
		}
		semdelete(done_sem);
		
		//print time taken
		uint32 later;
		gettime(&later);
		printf("Elapsed Seconds: %d\n", later-now+1);
  		return SHELL_OK;
	}
    
}
