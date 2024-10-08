/*
 * This file contains all the defined words in the given assembly language.
 * It includes the registers, operations, and instructions that are used in this assembly code
 * Seperted by type.
 * Additionally, it includes all the variables that represent characteristics of the assembly language.
 * The goal of the file is to avoid "Magic Numbers" in the code, make the code more clear and readable
 * while also making it more moduler as it's enough to change one #define to change it in many places in the code.
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
/*The length of direct register adressing (*r1)*/
#define DIR_REG_LEN 2
/*The length of undirect register adressing (r1)*/
#define UNDIR_REG_LEN 3
/*A std line can have no more than 2 operands*/
#define MAX_OPES 2
#define BIG_LEN 1234
#define ERR -1
/*Constants to reprenst groups of operations based on number of operands*/
#define Z_OP 0
#define O_OP 1
#define T_OP 2
/* Constants for all possible label types */
#define IMME 0
#define DIR 1
#define UNDIR_REG 2
#define DIR_REG 3
#define NONE 4
/*Ranged allowed for numbers*/
#define RANGE 2048
#define DATA_RANGE 16384
/*Size of machine code in the machine*/
#define CODESIZE 4096
/*Types of Labels*/
#define DATA 1
#define STD 2
#define EXTERN 3
#define ENTRY 4
#define BASE 10
/*Constant sizes, ENDING means the ".am" ending which needs to be removed*/
#define ENDING 3
#define FNAME_SZ 100
/*Bits positions for the final result*/
#define OPCODE_BIT 11
#define SRC_BIT 7
#define REG_SRC_BIT 6
#define DST_BIT 3
#define REG_DST_BIT 3
/*Type of addressing - Absolute, Relocatable or Extern*/
#define A 2
#define R 1
#define E 0
/*Numbers needed in the code*/
#define ADR_SP 3
#define THRESH 4
#define FIVE_OCT_DIG 077777
/* The arrays in our custom assembly language, defined extern to be accessible from different files in the project */
extern char *registers[];
extern char *operations[];
extern char *instructions[];




#endif /* ASSEMBLYCONSTANTS_H */
