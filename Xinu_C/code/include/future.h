#ifndef _FUTURE_H_
#define _FUTURE_H_

typedef enum {
  FUTURE_EMPTY,
  FUTURE_WAITING,
  FUTURE_READY
} future_state_t;

typedef enum {
  FUTURE_EXCLUSIVE,
  FUTURE_SHARED,
  FUTURE_QUEUE
} future_mode_t;

typedef struct {
  int value;
  future_state_t state;
  future_mode_t mode;
  
  //used for the EXCLUSIVE and SHARED types to block other processes from restricted activity
  pid32 get_pid;
  pid32 set_pid;
  
  //my queues are arrays with two pointers to the first and last elements
  int32 set_queue [NPROC];
  int32 get_queue [NPROC];
  int32 set_first;
  int32 get_first;
  int32 set_last;
  int32 get_last;
} future_t;

// Interface for the Futures system calls 
future_t* future_alloc(future_mode_t mode);
syscall future_free(future_t*);
syscall future_get(future_t*, int*);
syscall future_set(future_t*, int);

// Producer and consumer of futures
uint future_prod(future_t*, int);
uint future_cons(future_t*);
 
//Futures queue functions
pid32 enqueue_set(pid32, future_t*);
pid32 enqueue_get(pid32, future_t*);
pid32 dequeue_set(future_t*);
pid32 dequeue_get(future_t*);
 
#endif /* _FUTURE_H_ */