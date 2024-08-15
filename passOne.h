#ifndef PASSONE_H
#define PASSONE_H
#include "structs.h"




/*
 * The main function of the first pass
 * It gets the file name, the table of macros, table of labels and DC counter (initially 0)
 * it procceses the parsed file (after parsing the macros), finds errors in the syntax and building the full label table. Also it saves the final value of DC (as it's a pointer)
 * it returns the error flag, which will be 1 if there were no errors, otherwise 0
 */
int pass_one(char *filename, MacroTable *macro_table, LabelTable *label_table, int *DC);

/*
 * This function gets a line, a pointer to a label and to instruction
 * It finds everything that comes after the label and the instruction - it should be data or opearnds
 * It returns this as string
 */
char* get_line_remainder(char *line, char **label, char **instruction);

/*This function gets the line, the label, the instruction, everything after the instruction (the remainder), the macro and label tables, and the memory counters IC and DC
 * It validates and checks all the rules that were described in the booklet for a line, and returns 1 if the line is valid, otherwise 0
 */
int validate_line(char *line, char *label, char *instruction, char *remainder, int lineNum,MacroTable *macro_table, LabelTable *label_table, int IC, int DC) ;

/*
 * This function gets a label the table of macros
 * it returns 1 if the name is label name is valid, according to all the rules described in the booklet
 * otherwise returns 0
 */
int valid_label(char *label, MacroTable *macro_table);


/*This function gets a line and pointer to a label
 * It processes the line, getting the label from it and saving it in the variable given by the poiner
 * It returns the rest of the line after the label
 */
char* process_label(char *line, char **label);

/*This function gets a line and pointer to an instruction
 * It processes the line, getting the instruction from it and saving it in the variable given by the poiner
 * It returns the rest of the line after the instruction
 */
char* process_instruction(char *line, char **instruction);
/*
 * This function gets a string that reprensts an instruction (what comes after a label)
 * It returns 1 if it's equal to an operation or instruction name, which is a must
 * otherwise it returns 0 (non valid instruction)
 */
int valid_instruction(char *instruction);


/*This function gets a string and delets all the spaces from the end of the string*/
void delete_end_space(char *str);


/*
 * This function creates and allocates memory for an empty table of labels
 */
LabelTable* create_label_table();

/*
 * This function gets the table of labels and a label name, and returns 1 if the label is present in the table, otherwise 0
 */
int find_label(LabelTable *table, char *name);

/*
 * This function gets the table of labels and a label name, and returns its address as written in the table
 */
int get_address(LabelTable *table, char *name);


/* This function gets the table of labels, name of the label, the instruction that comes after the label, its address and the DC address
 *It allocates memory for the label, and adds it to the table with its attributes including address (that depends on type of instruction: IC\DC)
 */
void add_label(LabelTable *table, char *name, char *instruction, int address, int daddress);


/* This function gets the table of labels and frees the memory allocated for the label table
 * Doesn't return anything
 */
void free_label_table(LabelTable *table);




#endif /*PASSONE_H*/
