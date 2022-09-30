#include "_threadsCore.h" //include threadsCore header
#include "_kernelCore.h" //include kernelCore header
#include "osDefs.h" //include osDefs header

#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"

int main( void ) 
{
	SystemInit();
	printf("Running L-OS-S...\r\n");
	
	setThreadingWithPSP(getNewThreadStack(STACK_SIZE));
	
	// Initialize the kernel
	kernelInit();
	
	osSched();
	
	printf("here");
	while(1);
}
