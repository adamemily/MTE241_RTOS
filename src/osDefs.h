#ifndef  __OSDEFS_H

#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <stdbool.h>

//global constants
#define MAX_STACK 0x2000
#define STACK_SIZE 512
#define MAX_THREADS ((MAX_STACK - STACK_SIZE)/STACK_SIZE)

//define thread struct
typedef struct{
	uint32_t* TSP; //store thread stack pointer
	void (*fun_ptr)(void); //store thread's function pointer
} threadStruct;

#endif