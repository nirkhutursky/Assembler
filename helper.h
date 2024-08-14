

#ifndef HELPER_H
#define HELPER_H

#include <string.h>

#include "structs.h"


/*This functions adds .as ending to a given file
 * Gets the name of the input and output files
 * Changes the outputfile by pointer
 */
void get_out_name( char *input_filename, char *output_filename);

int is_label(char *line, char **label) ;

int is_instruction(char *line);

void prer(int lineNum, char* error_type);

int* parse_numbers(char *line, int *num_count);

char* parse_word(char *line);

int parse_operands(char *line, char **operand1, char **operand2);
int count_special_instruction(char *instruction, char *remainder, int lineNum);

int empty_s(char *str);

int count_commas(char *str);

int get_operand_type(char *oper, int lineNum);

int calc_IC(int type1, int type2);

void DC_mem_calc(LabelTable *label_table, int IC) ;

int valid_oper_oper(int op1,int op2, char *operation, int lineNum, int op_count);

signed int getNumber(char *op);

#endif /*HELPER_H*/
