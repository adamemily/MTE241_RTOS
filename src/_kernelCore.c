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
		threadCurr = 0;
		printf("%08x\n", (uint32_t) threadCollection[0].TSP);
		__set_CONTROL(1<<1); //enter threading
		__set_PSP((uint32_t) threadCollection[0].TSP); //set PSP to the first thread address

		osSched(); //start round-robin
	}
	
	return false; //once called, function should not end unless something went wrong in OS
}
	 
void osSched(void){ //TO DO: finish updating
	//identify which thread to switch to 
	if (numThreads > 1){
		threadCurr = (threadCurr+1)%numThreads;
	}
		//set PendSV interrupt to pending
			ICSR |= 1<<28;
		__asm("isb");
}

int task_switch(void){ //TO DO: verify function is correct once threadCurr is properly updated in osSched (to be written)
	//set the new PSP
	__set_PSP((uint32_t)threadCollection[threadCurr].TSP);
	
	return 0;
}