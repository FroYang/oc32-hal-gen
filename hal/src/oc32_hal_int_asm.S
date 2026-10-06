/* PLEASE DO NOT CHANGE THE FUNCTION NAME */

#include "CV32S6015SR.h"



.section .stext, "ax"

.global __asm_int0
.global __asm_int1
.global __asm_int2
.global __asm_int3
.global __asm_int4
.global __asm_int5
.global __asm_int6
.global __asm_int7
.global __asm_int8
.global __asm_int9
.global __asm_int10
.global __asm_int11
.global __asm_int12
.global __asm_int13
.global __asm_int14
.global __asm_int15
.global __asm_int16
.global __asm_int17
.global __asm_int18
.global __asm_int19
.global __asm_int20
.global __asm_int21
.global __asm_int22
.global __asm_int23
.global __asm_int24
.global __asm_int25
.global __asm_int26
.global __asm_int27
.global __asm_int28
.global __asm_int29
.global __asm_int30
.global __asm_int31



/* implement functions by assembly code, just return if it is empty
   ASM int will be executed before C int, so we can do something with lower latency 
   for the models without full-registers-stack, only R6 is avaliable at first,
   please push the other GPRs(R9~31) into stack before use them */

__asm_int0:
/* int0 is the interrupt for LVD, HRF(hardware reset flags) and PLL, 
   always means some kind of (power/clock) error occurred */

/* if PLL is enable, disable PLL & switch to default clock for safe */
	TBSR	CLKCON,  PLLE
	/* bit = 0 --> R3 = 1 */
	JNZ		.L1,  R3

	CBSR	CLKCON,  PLLE
.L0:
	TBSR	CLKCON,  CSF
	/* bit = 0 --> R3 = 1 */
	JNZ		.L0,  R3
	CBSR	CLKCON,  UXCDRE
	CBSR	CLKCON,  XHOSCE
/* return and let the C handler(__int0) handles it */
.L1:
	J   	[R7]

__asm_int1:
	J   	[R7]

__asm_int2:
	J   	[R7]

__asm_int3:
	J   	[R7]

__asm_int4:
	J   	[R7]

__asm_int5:
	J   	[R7]

__asm_int6:
	J   	[R7]

__asm_int7:
	J   	[R7]

__asm_int8:
	J   	[R7]

__asm_int9:
	J   	[R7]


__asm_int10:
	J   	[R7]

__asm_int11:
	J   	[R7]

__asm_int12:
	J   	[R7]

__asm_int13:
	J   	[R7]

__asm_int14:
	J   	[R7]

__asm_int15:
	J   	[R7]

__asm_int16:
	J   	[R7]

__asm_int17:
	J   	[R7]

/* ADC0 interrupt entry */
__asm_int18:
	J   	[R7]

__asm_int19:
	J   	[R7]



__asm_int20:
	J   	[R7]

__asm_int21:
	J   	[R7]

__asm_int22:
	J   	[R7]

__asm_int23:
	J   	[R7]

__asm_int24:
	J   	[R7]

__asm_int25:
	J   	[R7]

__asm_int26:
	J   	[R7]

__asm_int27:
	J   	[R7]

__asm_int28:
	J   	[R7]

__asm_int29:
	J   	[R7]



__asm_int30:
	J   	[R7]

__asm_int31:
	J   	[R7]

