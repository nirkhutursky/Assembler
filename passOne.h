#ifndef PASSONE_H
#define PASSONE_H
#include "structs.h"





int pass_one(const char *filename, MacroTable *macro_table, LabelTable *label_table);
/*char* trim_label(const char *line, char **label);
char* trim_instruction(const char *line, char **instruction);*/
char* get_line_remainder(char *line, char **label, char **instruction);
int validate_line(char *line, char *label, char *instruction, char *remainder, int lineNum,MacroTable *macro_table, LabelTable *label_table, int IC, int DC) ;
void del_ending_spaces(char *str);
int valid_label(char *label, MacroTable *macro_table);
int valid_instruction(char *instruction);
int get_instruction(char *instruction);
LabelTable* create_label_table();
int find_label(const LabelTable *table, const char *name);
int get_address(LabelTable *table, char *name);
/* Function to add a label to the label table */
void add_label(LabelTable *table, const char *name, char *instruction, int address, int daddress);
/* Function to free the memory allocated for the label table */
void free_label_table(LabelTable *table);

/* Function to check if a line contains a label and extract it */

/* Function to update the current address based on the line */
int update_address(const char *line, int current_address);

/* Function to print all labels in the label table (for debugging) */
void print_labels(const LabelTable *table);

#endif /*PASSONE_H*/
