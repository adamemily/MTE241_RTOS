#include "_threadsCore.h" //include threadsCore header
#include "_kernelCore.h" //include kernelCore header

#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"

int main( void ) 
{
	SystemInit();
	printf("Running L-OS-S...\r\n");
	
	setThreadingWithPSP(getNewThreadStack(512));
	kernelInit();
	osSched();
	
	while(1);
}
