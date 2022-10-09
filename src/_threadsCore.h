#ifndef  __THREADSCORE_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>

//function prototypes
//Lab 1 - switching threads
uint32_t* getMSPInitialLocation (void); //Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getNewThreadStack (uint32_t offset); //Returns address of new a PSP with offset of "offset" bytes from MSP.
void setThreadingWithPSP (uint32_t* threadStack); // Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register

//Lab 2 - creating and running threads
//int osThreadNew(void (*fun_ptr)(void)); //allocate memory for thread stack, add to array

//osKernelStart - task: set up interrupts, create at least one thread, call scheduler

#endif