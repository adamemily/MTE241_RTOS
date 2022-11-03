#include "_kernelCore.h"
#include "osDefs.h"

//global variables
extern threadStruct threadCollection[MAX_THREADS];
extern threadStruct idleThread;
extern int numThreads;
int threadCurr = 0;
bool canInterrupt = true;
bool noThreads = false;

//uint32_t* chad = NULL;

//set priority of the PendSV interrupt
void kernelInit(void){
	SHPR3 |= 0xFF << 16;
}

//start running the kernel, i.e. the OS
bool osKernelStart(){
	threadCurr = 0;
	if(numThreads > 0)
	{
		__set_CONTROL(1<<1); //enter threading
		__set_PSP((uint32_t) threadCollection[threadCurr].TSP); //set PSP to the first thread address
		
		osLoadFirst(); //begin running threads
	}
	
	return false; //once called, function should not end unless something went wrong in OS
}

//start running the first thread, which will lead into context switching between all the threads
void osLoadFirst(){
		ICSR |= 1<<28;
		__asm("isb");
}

//schedule the next thread to run and call the context switcher
void osYield(void){ 
	canInterrupt = false;
	
	//move TSP of the running thread 16 memory locations lower, so that next time the thread loads the 16 context registers, we end at the same PSP
	threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-16*4);
	if(threadCollection[threadCurr].sleepTime != 0){
		threadCollection[threadCurr].status = SLEEPING;
		threadCollection[threadCurr].timer = threadCollection[threadCurr].sleepTime;
	}
	else{
		threadCollection[threadCurr].status = WAITING;
	}
	
	//call scheduler
	scheduler();
	
	canInterrupt = true;
	ICSR |= 1<<28;
	__asm("isb");
}

void scheduler(void){
	noThreads = true;
	
	if (numThreads > 1){
		for (int i = 0; i < numThreads && noThreads == true; i++){
			//cycle through the threads in the thread struct array
			threadCurr = (threadCurr+1)%numThreads;
			printf("Trying thread %d\n", threadCurr+1);
			
			//check status: if next thread in round robin is waiting, proceed! else, loop back and look at next thread
			if(threadCollection[threadCurr].status == WAITING){
				noThreads = false;
			}
		}
		
		//if no threads are waiting, use idle thread
		if(noThreads == true){
			printf("all the people of the world are asleep\n");
		}

	}
}

void SysTick_Handler(void){
	//decrement running timeslice
	//printf("noThreads val: %d\n", noThreads);
	//printf("idle timer val: %d\n", idleThread.timer);
	if(noThreads == false){
		threadCollection[threadCurr].timer -= 1;
	}
	else{
		idleThread.timer -= 1;
	}
	
	//decrement sleep timers
	for(int i = 0; i < numThreads; i++){
		if(threadCollection[i].status == SLEEPING && i != threadCurr)
		{
			--threadCollection[i].timer;
			//check wake-up status
			if(threadCollection[i].timer == 0)
			{
				noThreads = false;
				threadCollection[i].status = WAITING;
				threadCollection[i].timer = threadCollection[i].timeslice; //assuming all timeslices for running threads are the same
			}
		}
	}
	
	//if idleThread timeslice is up, attempt task-switch
	if(idleThread.timer <= 0 && noThreads == true && canInterrupt == true){
		printf("idleThread timeslice complete... attempting re-entry\n");
		idleThread.TSP = (uint32_t*)(__get_PSP()-8*4);
		idleThread.status = WAITING;
		idleThread.timer = idleThread.timeslice;
		
		//call scheduler 
		scheduler();
	
		//call context switching routine, which will use a PSP to a new thread when it starts loading in register contents (updated threadCurr)
		ICSR |= 1<<28;
		__asm("isb");
	}
	//if timeslice of running thread is up AND thread is not currently yielding, proceed with task-switching
	else if(threadCollection[threadCurr].status == ACTIVE && threadCollection[threadCurr].timer <= 0 && canInterrupt == true)
	{
		printf("Thread %d timer complete.\n", threadCurr+1);
		threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-8*4);
		//prepare current thread to sleep if can sleep
		if(threadCollection[threadCurr].sleepTime != 0){
			threadCollection[threadCurr].status = SLEEPING;
			threadCollection[threadCurr].timer = threadCollection[threadCurr].sleepTime;
		}
		else{
			threadCollection[threadCurr].status = WAITING;
			threadCollection[threadCurr].timer = threadCollection[threadCurr].timeslice;
		}
		 
		//call scheduler 
		scheduler();
		
		//call context switching routine, which will use a PSP to a new thread when it starts loading in register contents (updated threadCurr)
		ICSR |= 1<<28;
		__asm("isb");
	}
}


int task_switch(void){
	//set PSP to the thread we want to start running
	if(noThreads == true){
		__set_PSP((uint32_t)idleThread.TSP);
		idleThread.status = ACTIVE;
		printf("idle here\n");
	}

	else{
		__set_PSP((uint32_t)threadCollection[threadCurr].TSP);
		threadCollection[threadCurr].status = ACTIVE;
		//printf("SET THREAD %d ACTIVE\n", threadCurr+1);
		printf("thread here\n");
	}
	
	return 0;
}
