.ORIG x3000
		LEA R6, stack		; load SP
		LDW R6, R6, #0
		LEA R1, arrloc
		LDW R1, R1, #0
		LEA R2, length
		AND R4, R4, #0
		LDW R2, R2, #0
LOOP 	BRz DONE
		LDB R3, R1, #0
		ADD R4, R4, R3		; compute sum
		ADD R1, R1, #1
		ADD R2, R2, #-1
		BR LOOP
DONE	STW R4, R1, #0		; store sum

		JMP R4				; exception

	HALT

arrloc 	.FILL xC000
length 	.FILL #20
stack	.FILL xFE00
.END