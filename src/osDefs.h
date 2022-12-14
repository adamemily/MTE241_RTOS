#ifndef  __OSDEFS_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <stdbool.h>

//global constants
#define MAX_STACK 0x2000
#define STACK_SIZE 512
#define MAX_THREADS ((MAX_STACK - STACK_SIZE)/STACK_SIZE)

#define ACTIVE 0
#define WAITING 1
#define SLEEPING 2

#define DEADLINE_DEFAULT 1000
#define DEADLINE_IDLE 500

#define YIELD_SWITCH 0
#define SYSTICK_SWITCH 1

//define thread struct
typedef struct{
	uint32_t* TSP; //store thread stack pointer
	void (*fun_ptr)(void); //store thread's function pointer
	
	int status; 
	int timer; //in ms
	int period;
	int deadline;

} threadStruct;

#endif