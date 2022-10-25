#include "_threadsCore.h" //include threadsCore header
#include "_kernelCore.h" //include kernelCore header
#include "osDefs.h" //include osDefs header

#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"
#include "LED.h"

int thread1Call = 0;
int thread2Call = 0;
int thread3Call = 0;


void thread1(){
	while(1){
		thread1Call++;
		
		//turn on LED 1 but keep others off
		if ((thread1Call % 3000) <= 1000){
			printf("This is a pause");

			LED_set(1);
			LED_clear(3);
			LED_clear(5);
		}
	
		osSched();
	}
}

void thread2(){
	while(1){
		thread2Call++;
		
		//turn on LED 3 but keep others off
		if ((thread1Call % 3000) > 1000 && (thread1Call % 3000) <= 2000){
			printf("This is a pause");
			LED_clear(1);
			LED_set(3);
			LED_clear(5);
		}
		osSched();
	}
}

void thread3(){
	while(1){
		thread3Call++;
		
		//turn on LED 5 but keep others off
		if ((thread1Call % 3000) > 2000){
			printf("This is a pause");
			LED_clear(1);
			LED_clear(3);
			LED_set(5);
		}
		osSched();
	}
}

int main( void ) 
{
	SystemInit();
	printf("\nRunning L-OS-S...\r\n");
	
	LED_setup();
	LED_clear(0);
	LED_clear(1);
	LED_clear(2);
	LED_clear(3);
	LED_clear(4);
	LED_clear(5);
	LED_clear(6);
	LED_clear(7);
	
	// Initialize the kernel
	kernelInit();

	osThreadNew(thread1);
	osThreadNew(thread2);
	osThreadNew(thread3);

	
	osKernelStart();
	
	printf("L-OS-S is lost (done)");
	while(1);
}
