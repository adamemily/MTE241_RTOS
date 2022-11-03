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
//test constants
const int TEST_RANGE = 3000;
const int TEST_INTERVAL1 = 1000;
const int TEST_INTERVAL2 = 2000;

extern threadStruct threadCollection[MAX_THREADS];
extern int threadCurr;

//thread functions
void thread1(){
	while(1){
		thread1Call++;
		
		printf("Running thread 1. Call count: %d\n", thread1Call);
		
		/*
		//turn on LED 1 but keep others off
		if ((thread1Call % TEST_RANGE) <= TEST_INTERVAL1){
			printf("This is a pause");

			LED_set(1);
			LED_clear(3);
			LED_clear(5);
		}
		*/
	
		osYield(); //call yield/scheduler function
	}
}

void thread2(){
	while(1){
		thread2Call++;
		printf("Running thread 2. Call count: %d\n", thread2Call);

		/*
		//turn on LED 3 but keep others off
		if ((thread1Call % TEST_RANGE) > TEST_INTERVAL1 && (thread1Call % TEST_RANGE) <= TEST_INTERVAL2){
			printf("This is a pause");
			LED_clear(1);
			LED_set(3);
			LED_clear(5);
		}
		*/
		
		osYield(); //call yield/scheduler function
	}
}

void thread3(){
	while(1){
		thread3Call++;
		//printf("		Running thread 3. Call count: %d\n", thread3Call);
		//printf("Running thread 3. Timer: %d\n", threadCollection[threadCurr].timer);

		/*
		//turn on LED 5 but keep others off
		if ((thread1Call % TEST_RANGE) > TEST_INTERVAL2){
			printf("This is a pause HEHEHEHEHE");
			LED_clear(1);
			LED_clear(3);
			LED_set(5);
		}
		*/
	}
}

int main( void ) 
{
	SystemInit();
	printf("\nRunning L-OS-S...\r\n");
	
	//LED initialization (set directions to output, start with all LEDs turned off)
	LED_setup();
	for(int i = 0; i < 7; i++){
		LED_clear(i);
	}

	//Initialize the kernel
	kernelInit();

	//Initialize each thread
	osThreadNew(thread1, TIMESLICE_DEFAULT, 0); //will yield
	osThreadNew(thread2, TIMESLICE_DEFAULT, 5000); //will sleep
	osThreadNew(thread3, 4000, 0); //will be pre-empted
	
	SysTick_Config(SystemCoreClock/1000);

	//Start running the threads
	osKernelStart();
	
	printf("L-OS-S is lost (done)");
	while(1);
}
