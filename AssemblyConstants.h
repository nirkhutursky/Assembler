/*
 * This file contains all the defined words in the given assembly language.
 * It includes the registers, operations, and instructions that are used in this assembly code
 * Seperted by type.
 * Additionally, it includes all the global variables that represent characteristics of the assembly language.
 */


#ifndef ASSEMBLYCONSTANTS_H
#define ASSEMBLYCONSTANTS_H



/*Number of registers*/
#define NUM_OF_REGISTERS 8
/*Number of operations*/
#define NUM_OF_OPERATIONS 16
/*Number of instructions*/
#define NUM_OF_INSTRUCTIONS 4
/*The maximal possible line size, including the '\n' character */
#define LINE_SIZE 81
/*The maximal possible label size */
#define LABEL_SIZE 31


/* The registers in our custom assembly language */
static const char *registers[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};
/* The operations in our custom assembly language  */
static const char *operations[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* The instructions in our custom assembly language  */
static const char *instructions[] = {
    ".data", ".string", ".entry", ".extern"
};



#endif /* ASSEMBLYCONSTANTS_H */
