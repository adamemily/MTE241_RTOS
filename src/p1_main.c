//Include function header
#include "_threadsCore.h"

//Include kernelCore header
#include "_kernelCore.h"

//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"


//This is C. The expected function heading is int main(void)
int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc. If you don't call this
	//you may see some weird behaviour
	SystemInit();

	//Printf now goes to the UART, so be sure to have PuTTY open and connected
	printf("Hello, world!\r\n");

	// getNewThreadStack(512);
	
	setThreadingWithPSP(getNewThreadStack(512));
		
	/*
	kernelInit();
	osSched();
	*/
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird
	while(1);
}
