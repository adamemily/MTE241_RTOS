#ifndef  __KERNELCORE_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <stdbool.h>

#define SHPR3 *(uint32_t*)0xE000ED20 //location of the PendSV priority register
#define SHPR2 *(uint32_t*)0xE000ED1C // location of SVC priority register
#define ICSR *(uint32_t*)0xE000ED04 //location of the ISCR

void kernelInit(void); //initialize memory structures and interrupts necessary to run the kernel

bool osKernelStart(); //start running the kernel, i.e. the OS

void osCreateMutex();

void osAcquireMutex(int threadID, int mutexID);

void osLoadFirst(); //called by the kernel to start running the very first thread, before getting into context switching
	 
void osYield(void); //called by the kernel to schedule the next thread to run and call the context switcher

void SysTick_Handler(void); //called when thread's timeslice is up, calls scheduler to decide which thread to run next

void scheduler(void); //decides which thread to run next, based off round-robin logic

void SVC_Handler_Main(uint32_t *svc_args);

int task_switch(void); //called by the PendSV interrupt to set PSP to the next thread scheduled to run



#endif