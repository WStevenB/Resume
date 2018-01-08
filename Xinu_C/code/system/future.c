#include <xinu.h>

// places new future on heap and returns pointer
// initializes queues, pids, and pointers
future_t* future_alloc(future_mode_t mode) {
	future_t fut;
	fut.mode = mode;
	fut.state = FUTURE_EMPTY;
	fut.get_pid = -1;
	fut.set_pid = -1;
	fut.get_first = 0;
	fut.set_first = 0;
	fut.get_last = 0;
	fut.set_last = 0;
	int i = 0;
	while(i < NPROC) {
		fut.get_queue[i] = -1;
		fut.set_queue[i] = -1;
		i++;
	}
	
	char* p = getmem(sizeof(fut));
	return (future_t*) memcpy(p, &fut, sizeof(fut));
}

// frees future from heap
syscall future_free(future_t* future) {
	return freemem(future, sizeof(*future));
}
 

syscall future_get(future_t* future, int* n) {
	int32 first_waiting = -1;
	
	//block interrupts to ensure checking and setting future locks happen atomically
	intmask mask = disable();
	
	switch(future->mode) {
	
	
		case FUTURE_EXCLUSIVE:

			switch(future->state) {
			
				//claims future by assigning its get_pid to itself
				//moves future to waiting
				//changes own state to PR_RECV, and calls resched
				case FUTURE_EMPTY:
					future->get_pid = currpid;
					future->state = FUTURE_WAITING;
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					//execution will eventually return here after resched()
					//recursive call to start function over
					future_get(future, n);
				break;
				
				//if get_pid id unassigned, claims it
				//otherwise returns error
				//changes own state to PR_RECV, and calls resched
				case FUTURE_WAITING:
					if(future->get_pid == -1) future->get_pid = currpid;
					if(currpid != future->get_pid) return SYSERR;
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					//execution will eventually return here after resched()
					//recursive call to start function over
					future_get(future, n);
				break;
				
				//if future's get_pid doesn't match, returns error
				//otherwise reads future's value
				case FUTURE_READY:
				
					//restore interrupts
					restore(mask);
					
					if(currpid != future->get_pid) return SYSERR;
					*n = future->value;
				break;
			}
		break;
		
		
		
		case FUTURE_SHARED:
			
			switch(future->state) {
			
				//enqueues itself into future's get queue
				//moves future to waiting state
				//changes own state to PR_RECV, and calls resched
				case FUTURE_EMPTY:
					enqueue_get(currpid, future);
					future->state = FUTURE_WAITING;
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					//execution will eventually return here after resched()
					//recursive call to start function over
					future_get(future, n);
				break;
				
				//enqueues itself into future's get queue
				//changes own state to PR_RECV, and calls resched
				case FUTURE_WAITING:
					enqueue_get(currpid, future);
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					//execution will eventually return here after resched()
					//recursive call to start function over
					future_get(future, n);
				break;
				
				//reads future's value
				case FUTURE_READY:
				
					//restore interrupts
					restore(mask);
					
					*n = future->value;
				break;
			}
		break;
		
		
		
		case FUTURE_QUEUE:
		
			switch(future->state) {
			
				//enqueues itself into future's get queue
				//moves future to waiting state
				//changes own state to PR_RECV, and calls resched
				case FUTURE_EMPTY:
					enqueue_get(currpid, future);
					future->state = FUTURE_WAITING;
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					//execution will eventually return here after resched()
					//recursive call to start function over
					future_get(future, n);
				break;
				
				//checks future's set queue
				//if empty (-1), enqueues itself to future's get queue and calls resched()
				case FUTURE_WAITING:
					first_waiting = dequeue_set(future);
					if(first_waiting == -1) {
						enqueue_get(currpid, future);
						
						//restore interrupts
						restore(mask);
					
						proctab[currpid].prstate = PR_RECV;
						resched();
					
						future_get(future, n);
					}
					//otherwise will wake up set process
					else {
						enqueue_get(currpid, future);
						
						//restore interrupts
						restore(mask);
					
						proctab[currpid].prstate = PR_RECV;
						if(proctab[first_waiting].prstate != PR_READY) ready(first_waiting);
					}
				break;
				
				//reads future's value
				//it more set processes are waiting, moves future to waiting state and wakes first one up
				case FUTURE_READY:
					future->state = FUTURE_WAITING;
					*n = future->value;
					first_waiting = dequeue_set(future);
					
					//restore interrupts
					restore(mask);
					
					if(first_waiting != -1) {
						if(proctab[first_waiting].prstate != PR_READY) ready(first_waiting);
					}
				break;
			}
		break;
	}
	return OK;
}

syscall future_set(future_t* future, int n) {
	int32 first_waiting = -1;
	
	//block interrupts to ensure checking and setting future locks happen atomically
	intmask mask = disable();
	
	switch(future->mode) {
	
	
		case FUTURE_EXCLUSIVE:

			switch(future->state) {
				
				//claims future by setting its set_pid to itself
				//moves future to ready state
				//sets its value
				case FUTURE_EMPTY:
					future->set_pid = currpid;
					future->state = FUTURE_READY;
					future->value = n;
					
					//restore interrupts
					restore(mask);
				break;
				
				//claims future if unclaimed, otherwise returns error
				//moves future to ready state and sets its value
				//wakes up get process
				case FUTURE_WAITING:
					if(future->set_pid == -1) future->set_pid = currpid;
					if(currpid != future->set_pid) return SYSERR;
					
					future->state = FUTURE_READY;
					future->value = n;
					
					//restore interrupts
					restore(mask);
					
					if(proctab[future->get_pid].prstate != PR_READY) ready(future->get_pid);
				break;
				
				//checks future's set_pid and returns error if not matching own pid
				//wakes up get process if it's not ready
				case FUTURE_READY:
					//restore interrupts
					restore(mask);
					
					if(currpid != future->set_pid) return SYSERR;
					if(proctab[future->get_pid].prstate != PR_READY) ready(future->get_pid);
				break;
			}
		break;
		
		
		
		case FUTURE_SHARED:
			
			switch(future->state) {
				
				//claims future by setting set_pid
				//sets future's value and moves it to ready state
				case FUTURE_EMPTY:
					future->set_pid = currpid;
					future->state = FUTURE_READY;
					future->value = n;
					
					//restore interrupts
					restore(mask);
				break;
				
				//claims future if unclaimed, otherwise returns error
				//moves future to ready state and sets its value
				//wakes up all queued get processes
				case FUTURE_WAITING:
					if(future->set_pid == -1) future->set_pid = currpid;
					if(currpid != future->set_pid) return SYSERR;
					
					future->state = FUTURE_READY;
					future->value = n;
					
					//restore interrupts
					restore(mask);
					
					first_waiting = dequeue_get(future);
					while(first_waiting != -1) {
						if(proctab[first_waiting].prstate != PR_READY) ready(first_waiting);
						first_waiting = dequeue_get(future);
					}
				break;
				
				//if no claim on this future, returns error
				//otherwise, sets future's value and wakes up all queued get processes
				case FUTURE_READY:
					//restore interrupts
					restore(mask);
					
					if(currpid != future->set_pid) return SYSERR;
					
					future->value = n;
					
					first_waiting = dequeue_get(future);
					while(first_waiting != -1) {
						if(proctab[first_waiting].prstate != PR_READY) ready(first_waiting);
						first_waiting = dequeue_get(future);
					}
				break;
			}
		break;
		
		
		
		case FUTURE_QUEUE:
		
			switch(future->state) {
			
				//moves future to waiting state
				//enqueues itself to set queue
				//changes own state to PR_RECV and calls resched()			
				case FUTURE_EMPTY:
					future->state = FUTURE_WAITING;
					enqueue_set(currpid, future);
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					//execution will eventually return here after resched()
					//recursive call to start function over
					future_set(future, n);
				break;
				
				
				//checks if there are waiting get processes
				//if empty (-1), enqueues self and reched()
				case FUTURE_WAITING:
					first_waiting = dequeue_get(future);
					if(first_waiting == -1) {
						enqueue_set(currpid, future);
						
						//restore interrupts
						restore(mask);
					
						proctab[currpid].prstate = PR_RECV;
						resched();
					
						future_set(future, n);
					}
					//otherwise, sets future's value/state and wakes up first queued get process
					else {
						future->state = FUTURE_READY;
						future->value = n;
						if(proctab[first_waiting].prstate != PR_READY) ready(first_waiting);
					}
				break;
				
				//does not set future value here
				//enqueues self and calls resched()
				case FUTURE_READY:
					enqueue_set(currpid, future);
					
					//restore interrupts
					restore(mask);
					
					proctab[currpid].prstate = PR_RECV;
					resched();
					
					future_set(future, n);
				break;
			}
		break;
	}
	return OK;
}



//producer for future
uint future_prod(future_t* fut,int n) {
  printf("Produced %d\n",n);
  future_set(fut, n);
  return OK;
}

//consumer of future values
uint future_cons(future_t* fut) {
  int i = 0, status = 0;
  status = (int)future_get(fut, &i);
  if (status == SYSERR) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Consumed %d\n", i);
  return OK;
}


//enqueues given pid into given future's set queue
pid32	enqueue_set(pid32 pid, future_t* fut) {
		
	fut->set_queue[fut->set_last] = pid;
	fut->set_last++;
	if(fut->set_last >= NPROC) fut->set_last = 0;
	return pid;
}
	
//enqueues given pid into given future's get queue
pid32	enqueue_get(pid32 pid, future_t* fut) {
		
	fut->get_queue[fut->get_last] = pid;
	fut->get_last++;
	if(fut->get_last >= NPROC) fut->get_last = 0;
	return pid;
}

//dequeues from given future's set queue
//returns -1 if empty
pid32	dequeue_set(future_t* fut) {

	if(fut->set_first == fut->set_last) return -1;
	
	pid32 pid = fut->set_queue[fut->set_first];
	fut->set_queue[fut->set_first] = -1;
	fut->set_first++;
	if(fut->set_first >= NPROC) fut->set_first = 0;
	return pid;
}
	
//dequeues from given future's get queue
//returns -1 if empty
pid32	dequeue_get(future_t* fut) {

	if(fut->get_first == fut->get_last) return -1;
	
	pid32 pid = fut->get_queue[fut->get_first];
	fut->get_queue[fut->get_first] = -1;
	fut->get_first++;
	if(fut->get_first >= NPROC) fut->get_first = 0;
	return pid;
}

