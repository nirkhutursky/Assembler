#ifndef PASSONE_H
#define PASSONE_H
#include "structs.h"




LabelTable* create_label_table();
void add_label(LabelTable *label_table, const char *name, int address, int type);
Label* find_label(LabelTable *label_table, const char *name);
void free_label_table(LabelTable *label_table);
int print_labels_content(const char *filename);

char* trim_label(const char *line, char **label);
char* trim_instruction(const char *line, char **instruction);
char* get_line_remainder(char *line, char **label, char **instruction);




#endif /*PASSONE_H*/
