/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    LD_EXC,
    LD_Priv,
    LD_Saved_USP,	
    LD_Saved_SSP,	
    LD_PC2,	
    LD_Vector,	
    GATE_PSR,	
    GATE_SP,	
    GATE_VECTOR,	
    GATE_PC2,
    PSR1MUX,	
    PSR2MUX,	
    SPMUX1, SPMUX0,		
    VECMUX,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetLD_EXC(int *x)           { return(x[LD_EXC]); }
int GetLD_Priv(int *x)        { return(x[LD_Priv]); }
int GetLD_Saved_USP(int *x)   { return(x[LD_Saved_USP]); }
int GetLD_Saved_SSP(int *x)   { return(x[LD_Saved_SSP]); }
int GetLD_PC2(int *x)        { return(x[LD_PC2]); }
int GetLD_Vector(int *x)     { return(x[LD_Vector]); }
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]); }
int GetGATE_SP(int *x)       { return(x[GATE_SP]); }
int GetGATE_VECTOR(int *x)   { return(x[GATE_VECTOR]); }
int GetGATE_PC2(int *x)      { return(x[GATE_PC2]); }
int GetPSR1MUX(int *x)       { return(x[PSR1MUX]); }
int GetPSR2MUX(int *x)       { return(x[PSR2MUX]); }
int GetSPMUX(int *x)         { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetVECMUX(int *x)        { return(x[VECMUX]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
int Priv; /* PSR[15] */
int Vector;
int Saved_USP;
int Saved_SSP;
/* MODIFY: You may add system latches that are required by your implementation */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);
    //printf("%d\n", CONTROL_STORE_BITS);
    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 2]);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.Priv = 1; /* Start in privileged mode */
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int SEXT(int value, int bits) {
    int sign_bit = (value >> (bits - 1)) & 0x1;
    if (sign_bit) {
        return value | (~0 << bits);
    }
    return value;
}

int ALU_output;
int SHF_output;
int MARMUX_output;
int ADDR_ADDER_output;
int MDR_output;
int PC2_output;
int PSR_output;
int SP_output;
int Vector_output;
int EXC=0;

void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */

   int *u_inst = CURRENT_LATCHES.MICROINSTRUCTION;

    if (CYCLE_COUNT == 299) {
        NEXT_LATCHES.INTV = 0x01;   // Set the interrupt vector
    }
    if (CURRENT_LATCHES.STATE_NUMBER == 54) {
        NEXT_LATCHES.INTV = 0x00;   // Clear the interrupt vector
        NEXT_LATCHES.EXCV = 0x00;   // Clear the exception vector
    }

    int OPCODE = (CURRENT_LATCHES.IR >> 12) & 0x0F;

    int EXC1 = ((CURRENT_LATCHES.MAR & 1) && GetDATA_SIZE(u_inst)) || ((CURRENT_LATCHES.MAR <= 0x2FFF) && CURRENT_LATCHES.Priv);
    int EXC0 = ((CURRENT_LATCHES.MAR <= 0x2FFF) && CURRENT_LATCHES.Priv) || (OPCODE == 10 || OPCODE == 11);
    EXC = (EXC1 << 1) + EXC0;

    int cond_bits = GetCOND(u_inst);
    
    if (GetIRD(u_inst) == 1) {
        // If IRD is 1, the next state is formed by the opcode in IR[15:12].
        NEXT_LATCHES.STATE_NUMBER = (OPCODE);
    } else if ((cond_bits == 6) && EXC1) { // EXC
        NEXT_LATCHES.STATE_NUMBER = 55; // Exception Handling State
    } else {
        int j_bits = GetJ(u_inst);
        //int cond_bits = GetCOND(u_inst);

        int next_state = j_bits;

        if (cond_bits == 1 && CURRENT_LATCHES.READY) { // READY
            next_state |= 0x2;
        } else if (cond_bits == 2 && CURRENT_LATCHES.BEN) { // BEN
            next_state |= 0x4;
        } else if (cond_bits == 3 && ((CURRENT_LATCHES.IR >> 11) & 0x1)) { // IR[11]
            next_state |= 0x1;
        } else if (cond_bits == 4 && (CURRENT_LATCHES.Priv)) { // PSR[15]
            next_state |= 0x8;
        } else if (cond_bits == 5 && (CURRENT_LATCHES.INTV)) { // INT
            next_state |= 0x10;
        } 

        NEXT_LATCHES.STATE_NUMBER = next_state;
    }
    
    // Fetch 
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

}


void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */

    static int mem_cycle_count = 0;
    int *micro = CURRENT_LATCHES.MICROINSTRUCTION;
    int mem_en = GetMIO_EN(micro);
    int r_w = GetR_W(micro);
    int data_size = GetDATA_SIZE(micro);
    //printf("Memory Cycle Count: %d\n", data_size);
    int mar = CURRENT_LATCHES.MAR;
    int mdr = CURRENT_LATCHES.MDR;
    int address = mar >> 1;

    if (mem_en) {
        mem_cycle_count++;
        NEXT_LATCHES.READY = 0;

        if (mem_cycle_count == 4) {
            NEXT_LATCHES.READY = 1; // latch READY
        }
        
        if (mem_cycle_count == 5) {
            // Perform the actual access
            if (r_w == 0) { // READ
                if (GetDATA_SIZE(micro) == 0) { // byte
                    int byte;
                    if (mar & 0x1) {
                        byte = (MEMORY[address][1]) & 0xFF;   // high-order byte
                        /* place the raw byte into the HIGH byte of MDR */
                        NEXT_LATCHES.MDR = Low16bits((byte << 8) & 0xFF00);
                    } else {
                        byte = (MEMORY[address][0]) & 0xFF;   // low-order byte
                        /* place the raw byte into the LOW byte of MDR */
                        NEXT_LATCHES.MDR = Low16bits(byte & 0x00FF);
                    }
                } 
                else { // word
                    NEXT_LATCHES.MDR = Low16bits((MEMORY[address][1] << 8) | MEMORY[address][0]);
                    //printf("Word read: 0x%.4x\n", NEXT_LATCHES.MDR);
                }
            } else { // WRITE
                if (data_size == 0) { // byte
                    if (mar & 0x1) MEMORY[address][1] = (mdr) & 0xFF;
                    else MEMORY[address][0] = mdr & 0xFF;
                } else { // word
                    MEMORY[address][0] = mdr & 0xFF;
                    MEMORY[address][1] = (mdr >> 8) & 0xFF;
                }
            }

            // Reset after completing access
            mem_cycle_count = 0;
            NEXT_LATCHES.READY = 0;
        }
    } else {
        // If memory not enabled, reset everything
        mem_cycle_count = 0;
        NEXT_LATCHES.READY = 0;
    }

}


void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    

    int *u_inst = CURRENT_LATCHES.MICROINSTRUCTION;
    int ir = CURRENT_LATCHES.IR;
    
    // 1. Calculate Address Adder Output
    int addr1_val, addr2_val, offset;
    addr1_val = GetADDR1MUX(u_inst) == 0 ? CURRENT_LATCHES.PC : CURRENT_LATCHES.REGS[(ir >> 6) & 0x07]; // ADDR1MUX
    
    switch (GetADDR2MUX(u_inst)) { // ADDR2MUX
        case 0: offset = 0; break;
        case 1: offset = SEXT(ir & 0x3F, 6); break;   // offset6 
        case 2: offset = SEXT(ir & 0x1FF, 9); break;  // PCoffset9 
        case 3: offset = SEXT(ir & 0x7FF, 11); break; // PCoffset11 
    }
    addr2_val = GetLSHF1(u_inst) == 1 ? (offset << 1) : offset; // LSHF1 
    ADDR_ADDER_output = addr1_val + addr2_val;

    // 2. Calculate MARMUX Output 
    MARMUX_output = GetMARMUX(u_inst) == 0 ? ((ir & 0xFF) << 1) : ADDR_ADDER_output; // MARMUX
    
    // 3. Calculate SHF Output 
    int sr1_shf = CURRENT_LATCHES.REGS[(ir >> 6) & 0x07];
    int amount4 = ir & 0x0F;
    if (((ir >> 4) & 0x01) == 0) { // LSHF 
        SHF_output = Low16bits(sr1_shf << amount4);
    } else { // RSHF 
        if (((ir >> 5) & 0x01) == 0) { // RSHFL (logical) 
            SHF_output = Low16bits((unsigned int)sr1_shf >> amount4);
        } else { // RSHFA (arithmetic) 
            //printf("sr1_shf: 0x%.4x, amount4: %d\n", sr1_shf, amount4);
            int sign = (sr1_shf & 0x8000) != 0;
            int shifted = (unsigned int)sr1_shf>> amount4;
            if(sign) {
                shifted = shifted | ~(((1 << (16 - amount4)) - 1)); 
            }
            SHF_output = Low16bits(shifted);
        }
    }
    
    // 4. Calculate ALU Output 
    int sr1 = (GetSR1MUX(u_inst) == 2) ? 6 : (GetSR1MUX(u_inst) ? ((ir >> 6) & 0x07) : ((ir >> 9) & 0x07));
    int sr1_alu = CURRENT_LATCHES.REGS[sr1]; /* SR1MUX */
    int sr2_alu;
    if ((ir >> 5) & 0x01) { // Immediate mode 
        sr2_alu = SEXT(ir & 0x1F, 5);
    } else { // Register mode 
        sr2_alu = CURRENT_LATCHES.REGS[ir & 0x07];
    }
    
    switch (GetALUK(u_inst)) { // ALUK 
        case 0: ALU_output = sr1_alu + sr2_alu; break; // ADD 
        case 1: ALU_output = sr1_alu & sr2_alu; break; // AND 
        case 2: ALU_output = sr1_alu ^ sr2_alu; break; // XOR 
        case 3: ALU_output = sr1_alu; break;           // PASSA
    }

    // 5. Calculate MDR Output 
    int mdr = CURRENT_LATCHES.MDR;
    if (GetDATA_SIZE(u_inst) == 0) { // BYTE 
        if (CURRENT_LATCHES.MAR & 0x1) { // High byte 
            MDR_output = SEXT((mdr >> 8) & 0xFF, 8);
        } else { // Low byte 
            MDR_output = SEXT(mdr & 0xFF, 8);  
        }
    } else { // WORD 
        MDR_output = Low16bits(mdr);
    }    

    // 6. Calculate PC-2
    PC2_output = Low16bits(CURRENT_LATCHES.PC - 2);

    // 7. Calculate PSR Output
    PSR_output = Low16bits((CURRENT_LATCHES.Priv << 15) | (CURRENT_LATCHES.N << 2) | (CURRENT_LATCHES.Z << 1) | CURRENT_LATCHES.P);
    switch (GetSPMUX(u_inst)) {
        case 0:
            SP_output = CURRENT_LATCHES.SSP;
            break;
        case 1:
            SP_output = Low16bits(CURRENT_LATCHES.REGS[sr1] - 2);
            break;
        case 2:
            SP_output = Low16bits(CURRENT_LATCHES.REGS[sr1] + 2);
            break;
        case 3:
            SP_output = CURRENT_LATCHES.Saved_USP;
    }

    // 8. Calculate Vector Output
    Vector_output = Low16bits(0x0200 + (CURRENT_LATCHES.Vector << 1));

}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       

   int *u_inst = CURRENT_LATCHES.MICROINSTRUCTION;

    if (GetGATE_PC(u_inst))         BUS = CURRENT_LATCHES.PC;
    else if (GetGATE_MDR(u_inst))       BUS = MDR_output;
    else if (GetGATE_ALU(u_inst))       BUS = ALU_output;
    else if (GetGATE_SHF(u_inst))       BUS = SHF_output;
    else if (GetGATE_MARMUX(u_inst))    BUS = MARMUX_output;
    else if (GetGATE_PC2(u_inst))       BUS = PC2_output;
    else if (GetGATE_PSR(u_inst))       BUS = PSR_output;
    else if (GetGATE_SP(u_inst))        BUS = SP_output;
    else if (GetGATE_VECTOR(u_inst))    BUS = Vector_output;
    else                                BUS = 0; /* No driver is active */

}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       
    int *u_inst = CURRENT_LATCHES.MICROINSTRUCTION;

    // Preserve values that are overwritten by the copy// Preserve values that are overwritten by the copy
    int next_state_number = NEXT_LATCHES.STATE_NUMBER;
    int next_microinstruction[CONTROL_STORE_BITS];
    memcpy(next_microinstruction, NEXT_LATCHES.MICROINSTRUCTION, sizeof(int)*CONTROL_STORE_BITS);
    int next_ready_bit = NEXT_LATCHES.READY;
    int next_mdr_val = NEXT_LATCHES.MDR; /* Crucial: Preserve MDR from cycle_memory */
    
    // Also preserve new Lab 4 latches set by other functions
    int next_intv = NEXT_LATCHES.INTV; 
    int next_excv = NEXT_LATCHES.EXCV;

    // All datapath latches hold their current values.
    NEXT_LATCHES = CURRENT_LATCHES;

    // RESTORE the preserved values that were overwritten by the copy 
    NEXT_LATCHES.STATE_NUMBER = next_state_number;
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, next_microinstruction, sizeof(int)*CONTROL_STORE_BITS);
    NEXT_LATCHES.READY = next_ready_bit;
    NEXT_LATCHES.MDR = next_mdr_val; /* Crucial: Restore MDR */
    
    // Restore new latches
    NEXT_LATCHES.INTV = next_intv;
    NEXT_LATCHES.EXCV = next_excv;

    // onditionally update the latches based on this cycle's control signals
    if (GetLD_MAR(u_inst))   NEXT_LATCHES.MAR = Low16bits(BUS);
    if (GetLD_IR(u_inst))    NEXT_LATCHES.IR = Low16bits(BUS);

    // LD.MDR signal loading from the BUS
    if (GetLD_MDR(u_inst) && (GetMIO_EN(u_inst) == 0)) {
        NEXT_LATCHES.MDR = Low16bits(BUS);
    }
    
    if (GetLD_BEN(u_inst)) {
        int n = (CURRENT_LATCHES.IR >> 11) & 0x01;  
        int z = (CURRENT_LATCHES.IR >> 10) & 0x01;
        int p = (CURRENT_LATCHES.IR >> 9) & 0x01;
        NEXT_LATCHES.BEN = (n && CURRENT_LATCHES.N) || (z && CURRENT_LATCHES.Z) || (p && CURRENT_LATCHES.P);
        //printf("BEN latched to %d\n", NEXT_LATCHES.BEN);
    }

    if (GetLD_REG(u_inst)) {
        int dr = GetDRMUX(u_inst) == 2 ? 6 : (GetDRMUX(u_inst) ? 7 : (CURRENT_LATCHES.IR >> 9) & 0x07);
        NEXT_LATCHES.REGS[dr] = Low16bits(BUS);
    }
    
    if (GetLD_CC(u_inst)) {
        if (GetPSR2MUX(u_inst)) { /* Loading from RTI */
            NEXT_LATCHES.N = (BUS >> 2) & 0x1;
            NEXT_LATCHES.Z = (BUS >> 1) & 0x1;
            NEXT_LATCHES.P = (BUS >> 0) & 0x1;
        } else { /* Loading from normal instruction */
            short signed_bus = (short)BUS;
            if (signed_bus < 0)       { NEXT_LATCHES.N = 1; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 0; }
            else if (signed_bus == 0) { NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 1; NEXT_LATCHES.P = 0; }
            else                      { NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 1; }
        }
    }

    if (GetLD_PC(u_inst)) {
        switch (GetPCMUX(u_inst)) {
            case 0: NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2; break;
            case 1: NEXT_LATCHES.PC = Low16bits(BUS); break;
            case 2: NEXT_LATCHES.PC = Low16bits(ADDR_ADDER_output); break;
        }
    }

    if (GetLD_Priv(u_inst)) {
        // printf("!!! Cycle %d: LD_Priv=1 detected in STATE %d. PSR1MUX=%d\n",
        //    CYCLE_COUNT + 1,
        //    CURRENT_LATCHES.STATE_NUMBER,
        //    GetPSR1MUX(u_inst));

        if (GetPSR1MUX(u_inst)) {
            NEXT_LATCHES.Priv = 0;
            //printf("setting priv to 0 at %d\n", CYCLE_COUNT);
        }
        else {
            NEXT_LATCHES.Priv = (BUS>>15) & 0x1;
        }
    }

    int sr1 = GetSR1MUX(u_inst) == 2 ? 6 : (GetSR1MUX(u_inst) ? ((CURRENT_LATCHES.IR >> 6) & 0x07) : ((CURRENT_LATCHES.IR >> 9) & 0x07));
    if (GetLD_Saved_USP(u_inst)) {
        if(GetSR1MUX(u_inst)==2) {
            NEXT_LATCHES.Saved_USP = CURRENT_LATCHES.REGS[6];
            //CURRENT_LATCHES.REGS[6] = CURRENT_LATCHES.Saved_SSP;
        }
    }
    if (GetLD_Saved_SSP(u_inst)) {
        if(GetSR1MUX(u_inst)==2) {
            NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[6];
            //CURRENT_LATCHES.REGS[6] = CURRENT_LATCHES.Saved_USP;
        }
    }
    if (GetLD_EXC(u_inst) && CURRENT_LATCHES.EXCV == 0) {
        switch (EXC) {
            case 1:
                NEXT_LATCHES.EXCV = 0x04;
                break;
            case 2:
                NEXT_LATCHES.EXCV = 0x03;
                break;
            case 3:
                NEXT_LATCHES.EXCV = 0x02;
        }
        EXC = 0; // Clear EXC after latching into EXCV
    }

    if (GetLD_Vector(u_inst)) {
        NEXT_LATCHES.Vector = GetVECMUX(u_inst) ? NEXT_LATCHES.EXCV : NEXT_LATCHES.INTV;
    }
}