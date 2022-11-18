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

		osYield(); //case 1&3
		//osSleep(50); //case 2
	}
}

void thread3(){
	while(1){
		thread3Call++;
		printf("Running thread 3. Call count: %d\n", thread2Call);
		
		//osYield(); 
	}
}

int main( void ) 
{
	SystemInit();
	printf("\nRunning L-OS-S...\r\n");
	

	//Initialize the kernel
	kernelInit();

	//Initialize each thread
	//CASE 1
	/*
	osThreadNew(thread1, 200, 4); //~256 Hz
	osThreadNew(thread2, 200, 10); //~100 Hz
	osThreadNew(thread3, 200, 83); //~12 Hz
	*/
	
	//CASE 2
	/*
	osThreadNew(thread1, 50, 0); //yields
	osThreadNew(thread2, 50, 0); //sleeps
	osThreadNew(thread3, 50, 4); //~250Hz
	*/
	
	//CASE 3
	
	osThreadNew(thread1, 200, 2000); //sleeps
	osThreadNew(thread2, 200, 3000); //sleeps
	
	
	osThreadNew(idleThread, DEADLINE_IDLE, 0); //always initialize last
	
	//Initialize frequency of SysTick_Handler
	SysTick_Config(SystemCoreClock/1000);

	
	//Start running the threads
	osKernelStart();
	
	printf("L-OS-S is lost (done)");
	while(1);
}
