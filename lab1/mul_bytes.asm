.ORIG x3000                                                                                                             
AND R3, R3, #0                                                                                                          
AND R4, R4, #0                                                                                                                                                                                                                                  
LEA R2, data                                                                                                           
LDB R0, R2, #0                                                                                                         
LDB R1, R2, #1    
                                                                                                                                                                                                                       
loop  ADD R3, R3, R0                                                                                                          
ADD R1, R1, #-1                                                                                                        
BRp loop                                                                                                                                                  
STB R3, R2, #2                                                                                                                                                                                                                                  
LDW R5, R2, #4                                                                                                          
AND R5, R3, R3                                                                                                          
BRz nooverflow                                                                                                                                                                                                                                  
ADD R4, R4, #1                                                                                                                                                                                                                                  
nooverflow STB R4, R3, #3                                                                                                                                                                                                                                  
HALT                                                                                                                                                                                                                                            
data .FILL x3100                                                                                                             
.FILL x3101
.FILL x3102
.FILL x3103
.FILL xFF00

.END
