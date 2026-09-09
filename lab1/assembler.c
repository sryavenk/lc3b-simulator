/*
	Name 1: Shrreya Venkatesan 
	Name 2: Justin Guo
	UTEID 1: sv28975
	UTEID 2: jmg9536 
*/


#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

void func();
int isOpcode(const char *tok);
int toNum();
int findSymbol();

#define MAX_LABEL_LEN 12    
#define MAX_SYMBOLS 255
typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];  /* Question for the reader: Why do we need to add 1? */
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

typedef struct {
    char *name;
    int opcode;
    int type; // 0 = ADD/AND/NOT, 1 = BR, 2 = JSR, 3 = JMP/JSRR/LD/LEA/LDB/LDW/ST/STB/STW, 4 = TRAP/.FILL/HALT/NOP
} Instructions;

Instructions opcodeTable[] = {
    {"add",  0x1, 0}, {"and", 0x5, 0}, {"not", 0x9, 0}, {"xor",  0x9, 0},

    {"br",   0x0, 1}, {"brn",  0x0, 1}, {"brz",  0x0, 1}, {"brp",   0x0, 1},
    {"brnz", 0x0, 1}, {"brnp", 0x0, 1}, {"brzp", 0x0, 1}, {"brnzp", 0x0, 1},

    {"jmp",  0xC, 3}, {"ret", 0xC, 3},
    {"jsr",  0x4, 2}, {"jsrr",0x4, 3},

    {"ldb",  0x2, 3}, {"ldw", 0x6, 3}, {"lea",  0xE, 3},
    {"stb",  0x3, 3}, {"stw", 0x7, 3},

    {"lshf", 0xD, 5}, {"rshfl",0xD, 5}, {"rshfa",0xD, 5},

    {"trap", 0xF, 4}, {"halt", 0xF025, 4}, {"nop",  0, 4},
    {".fill",0, 4}, {"rti",  0x8, 4}
};

FILE* infile = NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {
    //check for empty command line
     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;
    
     if(argc != 3){
        printf("Usage: %s <input> <output>\n", argv[0]);
        exit(1);
     }

     prgName   = argv[0];
     iFileName = argv[1];
     oFileName = argv[2];

     printf("program name = '%s'\n", prgName);
     printf("input file name = '%s'\n", iFileName);
     printf("output file name = '%s'\n", oFileName);

     /* open the source file */
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");

     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
                 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }
      /* Do stuff with files */

      func(infile);

     fclose(infile);
     fclose(outfile);
}

#define MAX_LINE_LENGTH 255
enum
{
    DONE, OK, EMPTY_LINE
};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
    {
        char * lRet, * lPtr;
        int i;
        if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
            return( DONE );
        for( i = 0; i < strlen( pLine ); i++ )
            pLine[i] = tolower( pLine[i] );








       /* convert entire line to lowercase */
          *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);








       /* ignore the comments */
           lPtr = pLine;








           while( *lPtr != ';' && *lPtr != '\0' &&
           *lPtr != '\n' )
                lPtr++;








           *lPtr = '\0';
           if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
                return( EMPTY_LINE );








           if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
           {
                *pLabel = lPtr;
                if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
           }
           
           *pOpcode = lPtr;








           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );








           *pArg1 = lPtr;








           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );








           *pArg2 = lPtr;
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );








           *pArg3 = lPtr;








           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );








           *pArg4 = lPtr;








           return( OK );
        }

int SymTabInx = 0;
int curradr;
int startLoc;
void func(FILE * lInfile) {
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
         *lArg2, *lArg3, *lArg4;
    int lRet;


    // Pass 1:
    do {
        lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if (lRet == DONE) {
            printf("Error: Assembly file is empty or missing .ORIG directive.\n");
            exit(2);
        }
    } while (strcmp(lOpcode, ".orig") != 0);

    // init address counter
    curradr = toNum(lArg1);

    // loop through file
    do {
        lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if (lRet != DONE && lRet != EMPTY_LINE) {
            // If a label exists on this line
            if (*lLabel != '\0') {
                symbolTable[SymTabInx].address = curradr;
                strcpy(symbolTable[SymTabInx].label, lLabel);
                SymTabInx++;
            }
            // advance counter
            if (isOpcode(lOpcode) != -1 || strcmp(lOpcode, ".fill") == 0) {
                curradr += 2;
            }
        }
    } while (lRet != DONE && strcmp(lOpcode, ".end") != 0);

    // Pass 2:
    rewind(lInfile);

    // Find .orig
    do {
        lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
    } while (strcmp(lOpcode, ".orig") != 0);

    // Reset the address counter for the second pass
    startLoc = toNum(lArg1);
    curradr = startLoc;
    fprintf(outfile, "0x%.4X\n", startLoc); // Write .orig address


    // machine cofe
    do {
        lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
       
        if (lRet != DONE && lRet != EMPTY_LINE) {
            // Skip  .ORIG line 
            if (strcmp(lOpcode, ".orig") == 0) {
                continue;
            }
            // end at .END
            if (strcmp(lOpcode, ".end") == 0) {
                break;
            }
           
            int instr = 0;

            // assembly logic
            for (int i = 0; i < sizeof(opcodeTable)/sizeof(opcodeTable[0]); i++) {
                if (!strcmp(lOpcode, opcodeTable[i].name)) {
                    instr = opcodeTable[i].opcode << 12;
                    switch(opcodeTable[i].type) {
                        case 0: // ADD, AND, XOR, NOT
                        {
                            int dr = lArg1[1] - '0';
                            int sr1 = lArg2[1] - '0';
                            if (strcmp(lOpcode, "not") == 0) {
                                instr |= (dr << 9) | (sr1 << 6) | 0x3F;
                            } else {
                                if (lArg3[0] == 'r') {
                                    int sr2 = lArg3[1]- '0';
                                    instr |= (dr << 9) | (sr1 << 6) | sr2;
                                } else {
                                    int imm5 = toNum(lArg3) & 0x1F;
                                    instr |= (dr << 9) | (sr1 << 6) | (1 << 5) | imm5;
                                }
                            }
                        }
                        break;
                        case 1: // BR 
                        {
                            int n = (strchr(lOpcode, 'n') != NULL);
                            int z = (strchr(lOpcode, 'z') != NULL);
                            int p = (strchr(lOpcode, 'p') != NULL);
                            if (strcmp(lOpcode, "br") == 0) n = z = p = 1;
                            int pcOffset9 = (findSymbol(lArg1) - (curradr + 2))/2;
                            instr |= (n << 11) | (z << 10) | (p << 9) | (pcOffset9 & 0x1FF);
                        }
                        break;
                        case 2: // JSR
                        {
                            if (!strcmp(lOpcode, "jsr")) {
                                int pcOffset11 = (findSymbol(lArg1) - (curradr + 2))/2;
                                instr |= (1 << 11) | (pcOffset11 & 0x7FF);
                            }
                        }
                        break;
                        case 3: // JMP, RET, JSRR, LEA, LDB, LDW, STB, STW
                        {
                            if (strcmp(lOpcode, "jmp") == 0 || strcmp(lOpcode, "ret") == 0) {
                                int baseR = (strcmp(lOpcode, "ret") == 0) ? 7 : (lArg1[1] - '0');
                                instr |= (baseR << 6);
                            } else if (strcmp(lOpcode, "jsrr") == 0) {
                                int baseR = lArg1[1] - '0';
                                instr |= (baseR << 6);
                            } else if (strcmp(lOpcode, "lea") == 0) {
                                int dr = lArg1[1] - '0';
                                int pcOffset9 = (findSymbol(lArg2) - (curradr + 2)) / 2;
                                instr |= (dr << 9) | (pcOffset9 & 0x1FF);
                            } else {
                                int r1 = lArg1[1]-'0';
                                int baseR = lArg2[1]-'0';
                                int offset6 = toNum(lArg3) & 0x3F;
                                instr |= (r1 << 9) | (baseR << 6) | offset6;
                            }
                        }
                        break;
                        case 4: // TRAP, HALT, NOP, .FILL
                        {
                            if (strcmp(lOpcode, "trap") == 0) {
                                int trapvec = toNum(lArg1) & 0xFF;
                                instr = (0xF0 << 8) | trapvec; // TRAP format is 1111 0000 trapvect8
                            } else if (strcmp(lOpcode, "halt") == 0) {
                                instr = 0xF025;
                            } else if (strcmp(lOpcode, "nop") == 0) {
                                instr = 0x0000;
                            } else if (strcmp(lOpcode, ".fill") == 0) {
                                instr = toNum(lArg1) & 0xFFFF;
                            }
                        }
                        break;
                        case 5: // Shifts (LSHF, RSHFL, RSHFA)
                        {
                            int dr = lArg1[1] - '0';
                            int sr = lArg2[1] - '0';
                            int amount4 = toNum(lArg3) & 0xF;
                            instr = (0xD << 12) | (dr << 9) | (sr << 6) | amount4;
                            if (strcmp(lOpcode, "rshfl") == 0) {
                                instr |= (1 << 4);
                            } else if (strcmp(lOpcode, "rshfa") == 0) {
                                instr |= (3 << 4);
                            }
                        }
                        break;
                    }
                    fprintf(outfile, "0x%.4X\n", instr);
                    break;
                }
            }
            // Increment the address counter for the next instruction.
            curradr += 2;
        }
    } while (lRet != DONE);
}

int isOpcode(const char *tok) {
     if(strcmp(tok, "add") == 0 || strcmp(tok, "and") == 0 ||
          strcmp(tok, "brn") == 0 || strcmp(tok, "brz") == 0 ||
          strcmp(tok, "brp") == 0 || strcmp(tok, "brnz") == 0 ||
          strcmp(tok, "brnp") == 0 || strcmp(tok, "brzp") == 0 ||
          strcmp(tok, "br") == 0 || strcmp(tok, "brnzp") == 0 ||
          strcmp(tok, "halt") == 0 || strcmp(tok, "jmp") == 0 ||
          strcmp(tok, "jsr") == 0 || strcmp(tok, "jsrr") == 0 ||
          strcmp(tok, "ldb") == 0 || strcmp(tok, "ldw") == 0 ||
          strcmp(tok, "lea") == 0 || strcmp(tok, "nop") == 0 ||
          strcmp(tok, "not") == 0 || strcmp(tok, "ret") == 0 ||
          strcmp(tok, "lshf") == 0 || strcmp(tok, "rshfl") == 0 ||
          strcmp(tok, "rshfa") == 0 || strcmp(tok, "rti") == 0 ||
          strcmp(tok, "stb") == 0 || strcmp(tok, "stw") == 0 ||
          strcmp(tok, "trap") == 0 || strcmp(tok, "xor") == 0) {
               return 1;
          }
          return -1;
}

int toNum( char * pStr ) {
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )               /* decimal */
   {
     pStr++;
     if( *pStr == '-' )             /* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
     printf("Error: invalid decimal operand, %s\n",orig_pStr);
     exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )  /* hex     */
   {
     pStr++;
     if( *pStr == '-' )             /* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
     printf("Error: invalid hex operand, %s\n",orig_pStr);
     exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
    printf( "Error: invalid operand, %s\n", orig_pStr);
    exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}


int findSymbol(const char *label) {
    for (int i = 0; i < SymTabInx; i++) {
        if (strcmp(symbolTable[i].label, label) == 0) {
            return symbolTable[i].address;  // found, return address
        }
    }
    return -1;  // not found
}
