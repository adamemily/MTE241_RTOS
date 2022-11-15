//inclusion of headers
#include "_threadsCore.h" 
#include "_kernelCore.h" 
#include "osDefs.h" 

//inclusion of libraries
#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"
#include "LED.h"

//global variables
int thread1Call = 0;
int thread2Call = 0;
int thread3Call = 0;
extern threadStruct threadCollection[MAX_THREADS];
extern int threadCurr;

//thread functions
void thread1(){
	while(1){
		thread1Call++;
		printf("Running thread 1. Call count: %d\n", thread1Call);
		
		osYield();
	}
}

void thread2(){
	while(1){
		thread2Call++;
		printf("Running thread 2. Call count: %d\n", thread2Call);

		osSleep(100); 
	}
}

void thread3(){
	while(1){
		thread3Call++;
		
		osYield(); 
	}
}

int main( void ) 
{
	SystemInit();
	printf("\nRunning L-OS-S...\r\n");
	

	//Initialize the kernel
	kernelInit();

	//Initialize each thread
	osThreadNew(thread1, 300, 0);
	osThreadNew(thread2, 200, 0);

	
	osThreadNew(idleThread, DEADLINE_IDLE, 0); //always initialize last
	
	//Initialize frequency of SysTick_Handler
	SysTick_Config(SystemCoreClock/1000);

	
	//Start running the threads
	osKernelStart();
	
	printf("L-OS-S is lost (done)");
	while(1);
}
