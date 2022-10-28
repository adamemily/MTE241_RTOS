#ifndef  __THREADSCORE_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>

//function prototypes
uint32_t* getMSPInitialLocation (void); //obtains the initial location of MSP by looking it up in the vector table

uint32_t* getNewThreadStack (uint32_t offset); //returns address of new a PSP with offset of "offset" bytes from MSP.

//void setThreadingWithPSP (uint32_t* threadStack); // sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register

int osThreadNew(void (*fun_ptr)(void), int timeSlice); //initializes the thread stack and its initial context in memory

#endif