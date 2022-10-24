#include "_threadsCore.h" //include threadsCore header
#include "_kernelCore.h" //include kernelCore header
#include "osDefs.h" //include osDefs header

#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"

int thread1Call = 0;

void thread1(){
	while(1){
		thread1Call++;
		//printf("This is Thread 1. Call number: %d\n'",thread1Call);
		osSched();
	}
}

int main( void ) 
{
	SystemInit();
	printf("\nRunning L-OS-S...\r\n");
	
	//setThreadingWithPSP(getNewThreadStack(STACK_SIZE));
	
	// Initialize the kernel
	kernelInit();

	osThreadNew(thread1);
	
	osKernelStart();
	
	printf("L-OS-S is lost (done)");
	while(1);
}
