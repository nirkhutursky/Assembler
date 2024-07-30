#ifndef PASSONE_H
#define PASSONE_H
#include "structs.h"




LabelTable* create_label_table();
void add_label(LabelTable *label_table, const char *name, int address, int type);
Label* find_label(LabelTable *label_table, const char *name);
void free_label_table(LabelTable *label_table);
int print_labels_content(const char *filename);


#endif /*PASSONE_H*/
