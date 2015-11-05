/* 
    File: scheduler.C

    Author: Hari Kishan Srikonda
	    Department of Electrical and Computer Engineering
            Texas A&M University
    Date  : 11/10/14

    Description: Thread scheduler.
                 Implemented Option 1

*/

#include "scheduler.H"
#include "console.H"
#include "machine.H"
#define SPIN_TIME 5000000

/* Two thread pointers for the ready queue*/
Thread * Scheduler::currThread;
Thread * Scheduler::nxtThread;

/* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed here as well. */
Scheduler::Scheduler()
{
  /* Initialize all pointers to zero */
  currThread = 0;
  nxtThread = 0;
}


void spin_wait(char* msg){
    Console::puts(msg);
    for(unsigned int j = 0; j < SPIN_TIME ; j++){
     asm("NOP");
    }
}

/* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'threads.h' to
      do the context switch. */
void Scheduler::yield()
{
  spin_wait("Please wait !! Yielding\n");
  /* Call Dispatch to give control to the next thread in the queue */
  Thread::dispatch_to(nxtThread);
}

/* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */
void Scheduler::resume(Thread * _thread)
{
  /* If it is the first thread give it control and define the next thread in the queue appropriately */
  if(currThread == 0){
  	currThread = _thread;
	currThread->next_thread = nxtThread;
  }
  /* If it is the last thread, then loop back to point to the first thread in the queue */
  else if(_thread->next_thread == 0){
	_thread->next_thread = currThread;
	nxtThread = currThread;
  }
  else
	nxtThread = _thread->next_thread;
}
   
/* Make the given thread runnable by the scheduler. This function is called
	  typically after thread creationl. Depending on the
      implementation, this may not entail more than simply adding the 
      thread to the ready queue (see scheduler_resume). */
void Scheduler::add(Thread * _thread)
{
  /* Temporary thread pointer to iterate through the queue */
  Thread * temp;
  /* If it is first thread, then the first add will be next thread */
  if(nxtThread == 0)
	nxtThread = _thread;

  else{
	temp = nxtThread;
	while(temp!= 0){
  		/* If the next thread is not defined, assign it and then come out of the loop */
		if(temp->next_thread == 0){
			temp->next_thread = _thread;
			break;
		}
	temp = temp->next_thread;
	}
  }  
}

/* Remove the given thread from the scheduler in preparation for destruction
      of the thread. */
void Scheduler::terminate(Thread * _thread)

{
  /* Temporary thread pointer to iterate through the queue */
  Thread * temp = currThread;
  while(temp!=0){
	/* Check if the one to be terminated is the next thread pointer */
	if(temp->next_thread == _thread){
		
		/* Disable interrupts for mutual exclusion while terminating thread */
  		machine_disable_interrupts();
		  /* Point Next thread pointer of current thread to the next->next thread pointer*/
		temp->next_thread = _thread->next_thread;
		  /* If the first thread is to be terminated make additional changes */
 		if(_thread == currThread){
			currThread = _thread->next_thread;
			nxtThread = currThread->next_thread;
		}
		else
			nxtThread = _thread->next_thread;
 		_thread = 0;
		/* Re-enable interrupts after terminating thread */
		machine_enable_interrupts();
		/* Call yield() in order to dispatch the next thread */
		Scheduler::yield();
	}
	temp = temp->next_thread;
  }
}
   
