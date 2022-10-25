#include "_kernelCore.h"
#include "osDefs.h"

extern threadStruct threadCollection[MAX_THREADS];
extern int numThreads;
int threadCurr = 0;

void kernelInit(void){
	SHPR3 |= 0xFF << 16;
}

bool osKernelStart(){
	if(numThreads > 0)
	{
		printf("your mom");
		printf("%08x\n", (uint32_t) threadCollection[threadCurr].TSP);
		
		threadCurr = 0;
		__set_CONTROL(1<<1); //enter threading
		__set_PSP((uint32_t) threadCollection[threadCurr].TSP); //set PSP to the first thread address
		
		osLoadFirst();
	}
	
	return false; //once called, function should not end unless something went wrong in OS
}

void osLoadFirst(){
		ICSR |= 1<<28;
		__asm("isb");
}
	 
void osSched(void){ 
	//move TSP of the running thread to the bottom of the register stack, so that next time the thread is loaded, we're in the right spot in memory
	threadCollection[threadCurr].TSP = (uint32_t*)(__get_PSP()-16*4);
	
	//identify which thread to switch to 
	if (numThreads > 1){
		threadCurr = (threadCurr+1)%numThreads;
	}
	
	//set PendSV interrupt to pending
	ICSR |= 1<<28;
	__asm("isb");
}

int task_switch(void){
	//set the new PSP
	__set_PSP((uint32_t)threadCollection[threadCurr].TSP);
	
	return 0;
}