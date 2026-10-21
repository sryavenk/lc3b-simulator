;Timer ISR

.ORIG x1200
STW R0, R6, #-1     ;save
STW R1, R6, #-2
LEA R0, Loc
LDW R0, R0, #0
LDW R1, R0, #0
ADD R1, R1, #1
STW R1, R0, #0
LDW R1, R6, #-2     ;restore
LDW R0, R6, #-1
RTI

Loc 	.FILL x4000
.END
