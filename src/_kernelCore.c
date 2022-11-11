#include "_kernelCore.h"
#include "osDefs.h"

//global variables
extern threadStruct threadCollection[MAX_THREADS];
extern int numThreads;
int threadCurr = 0;
extern int idleIndex;
bool leaveIdle = false;


//set priority of the PendSV interrupt
void kernelInit(void){
	//PendSV priority
	SHPR3 |= 0xFE << 16; 
	SHPR3 |= 0xFFU << 24; //SysTick priority
	
	SHPR2 |= 0xFDU << 24; //SVC priority
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

//called when a thread yields, starts task switching process
void osYield(void){ 
	//Call SVC
	__ASM("SVC #0");
	
	/*canInterrupt = false;
	
	//move TSP of the running thread 16 memory locations lower, so that next time the thread loads the 16 context registers, we end at the same PSP
	threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-16*4);
	//if the thread is able to sleep, set it to sleep
	if(threadCollection[threadCurr].sleepTime != 0){
		threadCollection[threadCurr].status = SLEEPING;
		threadCollection[threadCurr].timer = threadCollection[threadCurr].sleepTime; //set timer to user-defined sleep timer
	}
	//otherwise, set it back to waiting
	else{
		threadCollection[threadCurr].status = WAITING;
	}
	
	scheduler();
	
	canInterrupt = true;
	ICSR |= 1<<28;
	__asm("isb");*/
}

//determine next available thread to switch to
void scheduler(void){
	bool isFound = false;
	int index = threadCurr;
	int shortestDeadIndex = 0;
	
	if(numThreads > 0){
		for (int i = 0; i < numThreads; i++)
		{
			if (threadCollection[i].status == WAITING && (threadCollection[i].timer < threadCollection[shortestDeadIndex].timer)){
				isFound = true;
				shortestDeadIndex = i;
			}
		}
	}
		
	//if no threads are waiting, use idle thread
	if(isFound != true){
		printf("all the people of the world are asleep\n");
		threadCurr = idleIndex;
	}
	else{
		threadCurr = shortestDeadIndex;
	}
}

void SysTick_Handler(void){
	bool preEmptTask = false;
	//decrement all timers, deadlines and sleep timers
	for(int i = 0; i < numThreads; i++)
	{
		--threadCollection[i].timer;
		
		if(threadCollection[i].timer <= 0)
		{
			//check wake-up status if sleeping
			if(threadCollection[i].status == SLEEPING)
			{
				if(threadCurr == idleIndex){
						leaveIdle = true;
				}
				threadCollection[i].status = WAITING;
				threadCollection[i].timer = threadCollection[i].deadline;
				if (threadCollection[i].timer < threadCollection[threadCurr].timer){
					preEmptTask = true;
				}					
			}
			
			//check if system has failed if deadline has fniished
			if(threadCollection[i].status == WAITING){
				printf("DEADLINE MISSED :( \n");
			}
		}
	}
	
	//if timeslice of running thread is up, proceed with task-switching
	if(preEmptTask == true || leaveIdle == true)
	{
		threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-8*4); //decrement PSP only 8 locations lower, since the hardware registers remain on the stack
		threadCollection[threadCurr].status = WAITING;
		
		//if a thread woke up, return to EDF
		if(leaveIdle == true){
			threadCurr = idleIndex-1;
			leaveIdle = false;
		}
		 
		scheduler();
	
		ICSR |= 1<<28;
		__asm("isb");
	}
}

void SVC_Handler_Main(uint32_t *svc_args)
{
	char call = ((char*)svc_args[6])[-2];
	
	if(call == 0)
	{
		//move TSP of the running thread 16 memory locations lower, so that next time the thread loads the 16 context registers, we end at the same PSP
		threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-8*4);
		//if the thread is able to sleep, set it to sleep
		if(threadCollection[threadCurr].period != 0){
			threadCollection[threadCurr].status = SLEEPING;
			threadCollection[threadCurr].timer = threadCollection[threadCurr].period; //set timer to user-defined sleep timer
		}
		//otherwise, set it back to waiting
		else{
			threadCollection[threadCurr].status = WAITING;
			threadCollection[threadCurr].timer = threadCollection[threadCurr].deadline;
		}
		
		scheduler();
		
		ICSR |= 1<<28;
		__asm("isb");
	}
}


int task_switch(void){
	//set PSP to the thread we want to start running
	__set_PSP((uint32_t)threadCollection[threadCurr].TSP);
	threadCollection[threadCurr].status = ACTIVE;
	
	if (threadCurr == numThreads ){
		printf("Running idle thread \n");
	}
	return 0;
}


