#ifndef  __KERNELCORE_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <stdbool.h>

#define SHPR3 *(uint32_t*)0xE000ED20 //location of the PendSV priority register
#define ICSR *(uint32_t*)0xE000ED04 //location of the ISCR

void kernelInit(void); //initialize memory structures and interrupts necessary to run the kernel

bool osKernelStart(); //start running the kernel, i.e. the OS

void osLoadFirst(); //called by the kernel to load the very first thread, before getting into context switching
	 
void osSched(void); //called by the kernel to schedule which threads to run

int task_switch(void); //switch PSP, called in svc_call.s

#endif