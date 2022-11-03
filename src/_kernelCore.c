#include "_kernelCore.h"
#include "osDefs.h"

//global variables
extern threadStruct threadCollection[MAX_THREADS];
extern int numThreads;
int threadCurr = 0;

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
	

	ICSR |= 1<<28;
	__asm("isb");
}

void scheduler(void){
	bool isFound = false;
	
	if (numThreads > 1){
		for (int i = 0; i < numThreads && isFound == false; i++){
			//cycle through the threads in the thread struct array
			threadCurr = (threadCurr+1)%numThreads;
			printf("Trying thread %d\n", threadCurr+1);
			
			//check status: if next thread in round robin is waiting, proceed! else, loop back and look at next thread
			if(threadCollection[threadCurr].status == WAITING){
				isFound = true;
			}
		}
		
		//if no threads are waiting, use idle thread
		if(isFound != true){
			printf("all the people of the world are asleep\n");
		}
	}
}

void SysTick_Handler(void){
	//printf("SUSSY TICK CALLED ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
	//decrement running timeslice
	threadCollection[threadCurr].timer -= 1;
	//printf("thread num %d, timer: %d\n", threadCurr+1, threadCollection[threadCurr].timer);
	
	//decrement sleep timers
	for(int i = 0; i < numThreads; i++){
		if(threadCollection[i].status == SLEEPING && i != threadCurr)
		{
			--threadCollection[i].timer;
			//check wake-up status
			if(threadCollection[i].timer == 0)
			{
				threadCollection[i].status = WAITING;
				threadCollection[i].timer = threadCollection[i].timeslice; //assuming all timeslices for running threads are the same
			}
		}
	}
	
	//if timeslice of running thread is up, proceed with task-switching
	if(threadCollection[threadCurr].timer <= 0)
	{
		printf("Thread timeslice complete.\n");
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
	__set_PSP((uint32_t)threadCollection[threadCurr].TSP);
	threadCollection[threadCurr].status = ACTIVE;
	return 0;
}
