/* in file process_ring.c */

extern	process	polling_ring_process(volatile pid32 parent_pid, volatile int32 number_rounds, volatile int32 ring_id, volatile int32 waitFlag);

extern process semaphore_ring_process(volatile sid32 my_sem, volatile sid32 next_sem, volatile sid32 done_sem,
					 volatile int32 number_rounds, volatile int32 ring_id, volatile int32 counterPointer, volatile int32 waitFlag);

