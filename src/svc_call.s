	AREA	handle_pend,CODE,READONLY
	EXTERN task_switch 
	GLOBAL PendSV_Handler
	PRESERVE8
PendSV_Handler
		;Load PSP into r0
		MRS r0,PSP
		
		;Store the registers, decrementing in memory from the address stored in r0
		STMDB r0!,{r4-r11}
		
		;Call kernel task switch
		BL task_switch
		
		;Load into r0 the updated PSP, belonging to the new thread to run
		MRS r0,PSP
		MOV LR,#0xFFFFFFFD ;magic return value to get us back to Thread mode
		
		;LoaD Multiple Increment After. Load the context stored in the memory locations above the address stored in r0
		LDMIA r0!,{r4-r11}
		
		
		;Set PSP to the final location of r0
		MSR PSP,r0
		
		;Return
		BX LR

		END