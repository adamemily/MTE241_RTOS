#ifndef  __OSDEFS_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <stdbool.h>

//global constants
#define MAX_STACK 0x2000
#define STACK_SIZE 512
#define MAX_THREADS ((MAX_STACK - STACK_SIZE)/STACK_SIZE)

#define WAITING 0;
#define ACTIVE 1;
#define  BLOCKED 2;

//define thread struct
typedef struct{
	//store stack pointer, store function pointer
	uint32_t* TSP;
	void (*fun_ptr)(void);
	int readyStatus;
	int priority;
} threadStruct;

#endif