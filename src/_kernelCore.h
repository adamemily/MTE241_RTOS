#ifndef  __KERNELCORE_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <stdbool.h>

#define SHPR3 *(uint32_t*)0xE000ED20 //location of the PendSV priority register
#define ICSR *(uint32_t*)0xE000ED04 //location of the ISCR

void kernelInit(void); //initialize memory structures and interrupts necessary to run the kernel

bool osKernelStart(); //start running the kernel, i.e. the OS

void osLoadFirst(); //called by the kernel to start running the very first thread, before getting into context switching
	 
void osSched(void); //called by the kernel to schedule the next thread to run and call the context switcher

int task_switch(void); //called by the PendSV interrupt to set PSP to the next thread scheduled to run

#endif