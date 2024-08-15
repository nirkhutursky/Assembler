

#ifndef HELPER_H
#define HELPER_H

#include <string.h>

#include "structs.h"


/*This functions adds .as ending to a given file
 * Gets the name of the input and output files
 * Changes the outputfile by pointer
 */


/*This function gets a line number and type of error
 * and it prints an error message about this line with the type given
 */
void prer(int lineNum, char* error_type);

/*
 * This function gets a line that contains a list of numbers (after .data), and the amount of numbers in this list
 * It returns an array of ints that holds all the numbers in this list
 */
int* parse_numbers(char *line, int *num_count);


/*This function gets a line as a string
 * It finds the string that is saved inside "" (after .string)
 * and returns this string that is between the "   "
 * Returns error in case of error
 */
char* parse_word(char *line);


/*
 * This function gets a line and two pointers to operands
 * The function calcultes the value of the operands and saves them into the variables given
 * It returns the number of operands in the line
 */
int parse_operands(char *line, char **operand1, char **operand2);


/*
 * This function gets a special instruction (.data or .string), what comes after that (the remainder), and the line number
 * It validates that the data given after the instruction is valid, and returns its length (as it should increase DC by that number)
 */
int count_special_instruction(char *instruction, char *remainder, int lineNum);


/*
 * This function gets a string
 * And returns the number of commas in this string.
 * It's needed for the parsing functions
 */
int count_commas(char *str);

/*This function gets a string and returns 1 if it's empty, otherwise it returns 0*/
int empty(char *str);


/*
 * This function gets an operand and a line number, and validates it
 * If there is an error, it returns ERR and prints error message
 * otherwise it returns the addressing type of the operand
 */
int get_operand_type(char *oper, int lineNum);

/*
 * This function gets two integers that represent the types of both operands
 * It returns how many lines in the code are required to represent this line
 * It's needed because if both operands use addressing that uses registers, it requires one word less in the translation
 */
int calc_IC(int type1, int type2);

/*
 * This functions gets the label table and the final IC
 * It adds the IC value the all the labels that have type of DATA (that were defined after .data or .string)
 * It doesn't return anything
 */
void DC_mem_calc(LabelTable *label_table, int IC) ;

/*
 * This function gets two operand addressing types as ints, an operation, line number and number of operands
 * It returns 1 if the types are allowed in this operation (according to the table in the booklet), and 0 otherwise
 */
int valid_oper_oper(int op1,int op2, char *operation, int lineNum, int op_count);

/*
 * This function gets an operand that uses immediate addressing, and returns the number saved there
 * It returns it as a signed int so that if it's negative, it would be saved in 2's complement represnation in binary
 */
signed int getNumber(char *op);

#endif /*HELPER_H*/
