/*This file contains the functions and data structures related to the process of parsing the macros*/

#ifndef MACROPROCESSING_H
#define MACROPROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#include "AssemblyConstants.h"

/* Data Structures needed only for Macros*/

typedef struct Macro {
    char *name;
    Line *lines; /* Head of the linked list of lines */
} Macro;

/* Hash table node data structure */
typedef struct MacroNode {
    char *name;
    Macro *macro;
    struct MacroNode *next;
} MacroNode;

/* Hash table data structure */
typedef struct {
    MacroNode *table[MAX_NUM_OF_MACROS];
} MacroTable;
/*Function declarations*/

int parse_macros(const char *input_file, const char *output_file);
int is_macro_name_valid(const char *name,MacroTable *macro_table);
MacroTable* create_macro_table();
void insert_macro(MacroTable *macro_table, Macro *macro);
Macro* find_macro(MacroTable *macro_table, const char *name);
Macro* create_macro(const char *name);
void add_line_to_macro(Macro *macro, const char *line_value, int line_number);
void free_macro(Macro *macro);
unsigned int hash(const char *str);
char* read_line(FILE *file, char *buffer, int size, int *line_number);
int handle_macro_definition_start(MacroTable *macro_table, char *line, char *macro_name, int line_number);
void store_macro_definition(MacroTable *macro_table, const char *macro_name, char *line, int line_number);
void expand_macros(MacroTable *macro_table, char *line, FILE *out);
void write_expanded_line(char *line, FILE *out);

#endif /* MACROPROCESSING_H */

