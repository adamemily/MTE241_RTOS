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
int x = 0;
extern threadStruct threadCollection[MAX_THREADS];
extern int threadCurr;

//thread functions
void thread1(){
	while(1){
		//CASE 1
		osAcquireMutex(UART);
		thread1Call++;
		printf("Running thread 1. Call count: %d\n", thread1Call);
		osReleaseMutex(UART);
		
		//CASE 2
		/*
		osAcquireMutex(GLOBAL_X);
		x++;
		printf("Running thread 1. x = %d\n", x);
		osReleaseMutex(GLOBAL_X);
		*/
		
		osYield();
	}
}

void thread2(){
	while(1){
		//CASE 1
		osAcquireMutex(UART);
		thread2Call++;
		printf("Running thread 2. Call count: %d\n", thread2Call);
		osReleaseMutex(UART);
		osYield(); 
		
		//CASE 2
		/*
		osAcquireMutex(GLOBAL_X);
		osAcquireMutex(LEDS);
		
		LED_display(x%47);
		printf("Running thread 2\n");
		
		osReleaseMutex(LEDS);
		osReleaseMutex(GLOBAL_X);
		*/
	}
}

void thread3(){
	while(1){
		
		//CASE 1
		osAcquireMutex(UART);
		thread3Call++;
		printf("Running thread 3. Call count: %d\n", thread3Call);
		osReleaseMutex(UART);
		
		osYield(); 
		
		//CASE 2
		/*
		osAcquireMutex(LEDS);
		LED_display(0x71);
		printf("Running thread 3.\n");
		osReleaseMutex(LEDS);
		*/
	}
}

int main( void ) 
{
	SystemInit();
	printf("\nRunning L-OS-S...\r\n");
	
	//Clear all LEDs
	LED_setup();
	LED_clear(0);
	LED_clear(1);
	LED_clear(2);
	LED_clear(3);
	LED_clear(4);
	LED_clear(5);
	LED_clear(6);
	LED_clear(7);

	//Initialize the kernel
	kernelInit();

	//Initialize each thread
	osThreadNew(thread1, TIMESLICE_DEFAULT, 0);
	osThreadNew(thread2, TIMESLICE_DEFAULT, 0); 
	osThreadNew(thread3, TIMESLICE_DEFAULT, 0); 
	
	osThreadNew(idleThread, TIMESLICE_IDLE, 0); //always initialize last
	
	//Create each mutex
	osCreateMutex(); //UART
	osCreateMutex(); //GLOBAL_X
	osCreateMutex(); //LEDS
	
	//Initialize frequency of SysTick_Handler
	SysTick_Config(SystemCoreClock/1000);

	
	//Start running the threads
	osKernelStart();
	
	printf("L-OS-S is lost (done)");
	while(1);
}
