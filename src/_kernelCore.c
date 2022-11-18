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
	if(numThreads < 1){
		threadCurr = idleIndex;
	}
	
		ICSR |= 1<<28;
		__asm("isb");
}

//called when a non-periodic thread is set to sleep, starts task switching process
void osSleep(int sleepTime){
	threadCollection[threadCurr].status = SLEEPING;
	threadCollection[threadCurr].timer = sleepTime;
	
	printf("Thread yielded from osSleep.\n");
	__ASM("SVC #0");
}

//called when a thread yields, starts task switching process
void osYield(void){ 
	printf("Thread yielded from osYield.\n");
	__ASM("SVC #0");
}

//determine next available thread to switch to
void scheduler(void){
	bool isFound = false;
	int shortestDeadIndex = 0;
	
	if(numThreads > 0){
		for (int i = 0; i < numThreads; i++)
		{
			printf("Time on thread %d: %d, Status: %d\n", i+1, threadCollection[i].timer, threadCollection[i].status);
			
			//look for the earliest deadline among waiting tasks
			if(threadCollection[i].status == WAITING){
				if(isFound == false){ //at least one waiting task found
					isFound = true;
					shortestDeadIndex = i;
					
				}
				if(isFound == true){ //find earlier deadlines than the one found
					if(threadCollection[i].timer < threadCollection[shortestDeadIndex].timer){ //if there's a tie, stick with the thread of the lower index
						shortestDeadIndex = i;
					}
				}
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
		printf("Trying thread %d\n", threadCurr+1);
	}
}

void SysTick_Handler(void){
	bool preEmptTask = false;
	
	for(int i = 0; i < numThreads; i++)
	{
		--threadCollection[i].timer; //decrement all timers: deadlines and sleep timers
		
		if(threadCollection[i].timer <= 0) //i is within numThreads range, therefore does not include the idleThread
		{
			//thread failed to meet its deadline. user's fault for designing poor threads
			if(threadCollection[i].status == WAITING){
				printf("Deadline of thread %d missed -- system failed.\n", i+1);
			}
			
			//check wake-up status if sleeping
			if(threadCollection[i].status == SLEEPING)
			{
				printf("Waking up thread %d\n", i+1);
				
				if(threadCurr == idleIndex){
						leaveIdle = true;
				}
				threadCollection[i].status = WAITING;
				threadCollection[i].timer = threadCollection[i].deadline;
				
				//if the thread that has woken up has an earlier deadline than the running task, or has a tied deadline
					//if tied, choose the thread with the lower index
				if (threadCollection[i].timer < threadCollection[threadCurr].timer || (threadCollection[i].timer == threadCollection[threadCurr].timer && i < threadCurr)){
					preEmptTask = true;
				}
			}
		}
	}
	
	//leave current thread if preempted by another thread with a earlier deadline, have been signalled to leave the idle thread, or if the running thread's timer goes to zero
	if(preEmptTask == true || leaveIdle == true || threadCollection[threadCurr].timer <= 0)
	{
		printf("Thread yielded from SysTick.\n");
		
		threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-8*4); //decrement PSP only 8 locations lower, since the hardware registers remain on the stack
		
		if(preEmptTask != true){ //leaveIdle or timer = 0. don't reset the deadline if pre-empted
			//set to sleep if periodic
			if(threadCollection[threadCurr].period != 0){
				threadCollection[threadCurr].status = SLEEPING;
				threadCollection[threadCurr].timer = threadCollection[threadCurr].period;
			}
			else{
				threadCollection[threadCurr].status = WAITING;
				threadCollection[threadCurr].timer = threadCollection[threadCurr].deadline;
			}
		}
		else{
			threadCollection[threadCurr].status = WAITING;
			preEmptTask = false; //reset bool
		}
		
		//reset leaveIdle flag
		if(leaveIdle == true){
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
	
	if(call == 0) //code for a thread that has yielded/yielded by osSleep()
	{
		//move TSP of the running thread 8 memory locations lower, so that next time the thread loads the 16 context registers, we end at the same PSP
		threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-8*4);
		//in the case of a periodic thread that yields
		if(threadCollection[threadCurr].period != 0){
			threadCollection[threadCurr].status = SLEEPING;
			threadCollection[threadCurr].timer = threadCollection[threadCurr].period;
		}
		//if not periodic and if not yielded by osSleep
		else if(threadCollection[threadCurr].status != SLEEPING){
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
	
	if (threadCurr == idleIndex ){
		printf("Running idle thread \n");
	}
	return 0;
}


