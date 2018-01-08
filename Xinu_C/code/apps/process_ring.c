#include <xinu.h>

process semaphore_ring_process(volatile sid32 my_sem, volatile sid32 next_sem, volatile sid32 done_sem,
					 volatile int32 number_rounds, volatile int32 ring_id, volatile int32 counterPointer, volatile int32 waitFlag){
	//starting round
  	int32 round = 0;
  	//assign arg to local pointer
  	int* countdown = (int *) counterPointer;
  	
  	//countdown loop 
  	while (round < number_rounds){
  	
  		//wait until my semaphore is signaled
    	wait(my_sem);
    	
    	//print output
    	printf("Ring Element: %d\tRound: %d\tValue: %d\n", ring_id, round, *countdown);
    	
    	//if countdown should continue, decrement counter and signal next semaphore
    	if(*countdown != 0) {
    		 *countdown = *countdown - 1;
    		 signal(next_sem);
    	}
    	//increase round
    	round++;
  	}
  	//sleep until wait flag is clear to exit
  	int* shouldWait = (int *) waitFlag;
  	while(*shouldWait == 1) sleep(0.1);
  	//set wait flag to block other processes from exiting at same time
  	*shouldWait = 1;
  	//prompt parent process to receive exit code
  	signal(done_sem);
  	return OK;
} 

process polling_ring_process(volatile pid32 parent_pid, volatile int32 number_rounds, volatile int32 ring_id, volatile int32 waitFlag){
	//wait to be assigned id of next process in ring
	pid32 next_process = -1;
	while(next_process == -1) {
		next_process = receive();
	}
	//starting round
  	int32 round = 0; 
  	
  	//countdown loop
  	while (round < number_rounds){
  		//wait for message
    	int32 message = receive();
    	
    	//print output
    	printf("Ring Element: %d\tRound: %d\tValue: %d\n", ring_id, round, message);
    	
    	//if countdown should continue, signal next process
    	if(message != 0) send(next_process, message - 1);
    	
    	//increase current round
    	round++;
  	}
  	//sleep until wait flag is clear to exit
  	int* shouldWait = (int *) waitFlag;
  	while(*shouldWait == 1) sleep(0.1);
  	//wait for message then relay to next process
  	*shouldWait = 1;
  	//prompt parent process to receive exit code
  	send(parent_pid, OK);
  	return OK;
} 