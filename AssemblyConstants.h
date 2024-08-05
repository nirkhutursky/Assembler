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
#define LINE_SIZE 94
/*The maximal possible label size */
#define LABEL_SIZE 31
/*The maximum number of macros allowed in one file */
#define MAX_NUM_OF_MACROS 137
/*the length of the start of macro definition, starting with "macr "*/
#define MACRODEF 5
/*the length of the end of macro definition, starting with "endmacr"*/
#define MACEND 7
/*The starting space for labels in the label table*/
#define START_SIZE 8
/*The starting adress of the instruction counter*/
#define ADDRESS_START 100
#define BIG_LEN 1234
/* Constants for all possible label types */
#define DATA 1
#define STRING 2
#define ENTRY 3
#define EXTERN 4

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
