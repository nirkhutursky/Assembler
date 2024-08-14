

#ifndef PASSTWO_H
#define PASSTWO_H
#include "structs.h"


/*
 * This is the main function of the second pass. It goes over the file, validating and encoding each line to its correct value, and writing to the file the code, entries and externs
 * It gets the file name, the table of labels that were found in the first pass, flag that tells us whether we found a mistake in the first run
 * and DC, which is the data counter from the first pass and it's needed to put the data labels into the correct position in the machine code
 * It doesn't return anything, simply creating the three files needed - the machine code, the entries and the externs
 */
void pass_two(char *filename, LabelTable *label_table, int ErrorFlag,int DC);

/*
 * This function validates that entry labels were defined in the current file, and that extern labels were not
 * It gets the instruction, the label, the line number, the label table, and both operands
 * It returns 1 if the line is valid, otherwise 0
 */
int sec_pass_valid_line(char *instruction, char *remainder, int lineNum, LabelTable *label_table, char *op1, char *op2);

/*
 * This function gets an operation
 * And returns its opcode according to the table
 */
int get_opcode(char *op);

/*
 * This function gets an operation and the two types of operands
 * It returns the encoding of the operation including the addressing methods of the operands
 */
int encbin(char *instruction, int type1, int type2);


/*
 * This function gets an operand, its type, whether it's src or dst, and the label table
 * It returns the encoding of the operand, according to the rules
 */
int encbinoper(char *op, int type, int where, LabelTable *label_table);
#endif /*PASSTWO_H*/
