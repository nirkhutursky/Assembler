

#ifndef PASSTWO_H
#define PASSTWO_H
#include "structs.h"

void pass_two(char *filename, LabelTable *label_table, int ErrorFlag,int DC);

int sec_pass_valid_line(char *instruction, char *remainder, int lineNum, LabelTable *label_table, char *op1, char *op2);
int get_opcode(char *op);

int encbin(char *instruction, int type1, int type2);

int encbinoper(char *op, int type, int where, LabelTable *label_table);
#endif //PASSTWO_H
