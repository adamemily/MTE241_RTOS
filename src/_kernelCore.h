#ifndef  __KERNELCORE_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>

#define SHPR3 *(uint32_t*)0xE000ED20 //location of the PendSV priority register
#define ICSR *(uint32_t*)0xE000ED04 // Location of the ISCR

void kernelInit(void); //initializes memory structures and interrupts necessary to run the kernel
	 
void osSched(void); //called by the kernel to schedule which threads to run


#endif