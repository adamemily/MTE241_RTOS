#include "_threadsCore.h"

uint32_t max_stack = 0x2000;

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation (void){
	uint32_t* MSP_ptr = (uint32_t*) 0x0; //idefine a pointer to a pointer that points to initial MSP
	printf("%08x\n", *MSP_ptr);
	return (uint32_t*) *MSP_ptr; //dereference so that it is just the pointer 
}

//Returns address of new a PSP with offset of "offset" bytes from MSP.
uint32_t* getNewThreadStack (uint32_t offset){
	//check if we are exceeding the max stack size
	if (offset > max_stack){
		printf("ERROR: Offset too large");
		return NULL;
	}

	uint32_t* MSP_ptr = getMSPInitialLocation();
	uint32_t PSP_adr = (uint32_t) MSP_ptr - offset; //do arithemtic in integers
	uint32_t* PSP_ptr;
	*PSP_ptr = PSP_adr;
	printf("%08x\n", *PSP_ptr);
	return PSP_ptr;
}


// Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register
void setThreadingWithPSP (uint32_t* threadStack){
	__set_PSP(*threadStack);
	printf("%08x\n", *threadStack);
	__set_CONTROL(1<<1);
}