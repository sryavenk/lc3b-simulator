.ORIG x1200
		ADD R6, R6, #-2		;save vals in stack
		STW R0, R6, #0
		ADD R6, R6, #-2
		STW R1, R6, #0
		ADD R6, R6, #-2
		STW R2, R6, #0
	
		LEA R0, arrloc
		LEA R1, length
		LDW R0, R0, #0
		LDW R1, R1, #0
	
LOOP	LDW R2, R0, #0
		AND R2, R2, #-2
		STW R2, R0, #0
		ADD R0, R0, #2
		ADD R1, R1, #-1
		BRp LOOP
	
		LDW R2, R6, #0		;restore vals from stack
		ADD R6, R6, #2
		LDW R1, R6, #0
		ADD R6, R6, #2
		LDW R0, R6, #0
		ADD R6, R6, #2
		RTI
		
arrloc	.FILL x1000
length	.FILL #128
.END