/*This file contains the functions and data structures related to the process of parsing the macros*/

#ifndef MACROPROCESSING_H
#define MACROPROCESSING_H

#include <stdio.h>
#include "structs.h"





/* Function that checks if a macro name is valid (not an operation or instruction name,
 * and not an already defined macro, and only valid chars)
 * gets the macro name and macro table as input
 * returns 1 if valid, otherwise 0
 */
int validate_macro(const char *name, MacroTable *macro_table);


/* Function that creates the table of macros, initially it's empty.
 * gets the macro name and macro table as input
 * returns 1 if valid, otherwise 0
 */
MacroTable* create_macro_table();

/* Function that copies the content of a string into another string
 * gets the pointer to the string
 * returns returns a copy of this string
 */
char* copy(const char *s);

/* Function that inserts a macro into the macro table
 * gets a macro and the macro table as input
 * inserts it to the table with dynamic allocation
 */
void insert_macro(MacroTable *macro_table, Macro *macro);

/* Function that finds a macro inside the macro table
 * gets the macro name and macro table as input
 * returns the macro if found in the table, otherwise returns null
 */
Macro* find_macro(MacroTable *macro_table, const char *name);

/*This function creates a new macro with the given name, as a linked list of lines
 * It gets the name of the macro to create.
 * Returns a pointer to the newly created Macro (struct)
 */
Macro* create_macro(const char *name);

/*Adds a line to an existing macro.
 * It gets the macro which we want to add a line to, the line itself and its number
 */
void add_macro_line(Macro *macro, const char *line, int line_number);

/*Frees the memory allocated for a macro.
 * Gets the pointer to Macro we want to eliminate and free it from memory
 */
void free_macro(Macro *macro);

/*Reads a line from the file, and deletes all whitespaces at the start and the end
 * Gets the file, a buffer to save the line, size of the line the its number
 * Returns the clean line after deleting white spaces
 */
char* read_line(FILE *file, char *buffer, int size, int *line_number);

/*Processes a macro definition
 *It gets the table of macros, the line with the definition, the name of the macro and the line number
 *it validates the correctnes of the macro definition and saves it in the macro table
 *Returns 1 if the processing finished without issues, otherwise 0
 */
int process_macro(MacroTable *macro_table, char *line, char *macro_name, int line_number);

/*This functions adds data to a given macro, by adding a line to it.
 * It gets the macro table, the name of the macro, the line and its number
 * it finds the macro in the table ands adds the line to it
 */
void macro_data(MacroTable *macro_table, const char *macro_name, char *line, int line_number);

/* This function writes the lines in the output file. it expandes the macros when it notices them later in the code,
 * after they were defined. It writes them to the output file, and also writes any non macro line to the file.
 * It reads the line and checks whether it's a defined macro, if so it writes all its saved data to the file
 * It gets the macro table, the name of the macro, the line and its number
 */
void expand_macros(MacroTable *macro_table, char *line, FILE *out);

/*This function writes a line as it is to the output file, and adds a newline char
 * it gets the line to write, and the file the we write in
 */
void putLine(char *line, FILE *out);

/*This function is the main function, it parses the macros line by line
 * categorizing each line (either a macro definition, macro end, inside a macro or outside a macro
 * and saving each line to the correct place
 * It gets the input file from which it reads lines and processing them,
 * and it gets the output file to which we write the expanded macros. This will the processed file
 */
int parse_macros(const char *input_file, const char *output_file);

#endif /* MACROPROCESSING_H*/



