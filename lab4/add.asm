.ORIG x3000
		LEA R6, stack
		LDW R6, R6, #0
		AND R0, R0, #0
		ADD R0, R0, #1
		LEA R1, int
		LDW R1, R1, #0
		STB R0, R1, #0
		LEA R1, arrloc
		LDW R1, R1, #0
		LEA R2, length
		AND R4, R4, #0
		LDW R2, R2, #0
LOOP 	BRz DONE
		LDB R3, R1, #0
		ADD R4, R4, R3
		ADD R1, R1, #1
		ADD R2, R2, #-1
		BR LOOP
DONE	STW R4, R1, #0

; Unaligned exception
		ADD R1, R1, #3
		STW R4, R1, #0

	HALT
int 	.FILL x4000
arrloc 	.FILL xC000
length 	.FILL #20
stack	.FILL xFE00
.END