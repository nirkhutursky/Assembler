#include "MacroProcessing.h"
#include <ctype.h>
#include "structs.h"
#include "AssemblyConstants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Function that checks if a macro name is valid (not an operation or instruction name,
 * and not an already defined macro, and only valid chars)*/
int validate_macro(const char *name, MacroTable *macro_table) {
    int i;
    /* Check that it's not an operation name */
    for (i = 0; i < NUM_OF_OPERATIONS; ++i) {
        if (strcmp(name, operations[i]) == 0) {
            return 0; /* Invalid name */
        }
    }

    /* Check that it's not an instruction name */
    for (i = 0; i < NUM_OF_INSTRUCTIONS; ++i) {
        if (strcmp(name, instructions[i]) == 0) {
            return 0; /* Invalid name */
        }
    }
    /*Check that it's not an already defined macro*/
    if (find_macro(macro_table, name) != NULL) return 0;

    for (i = 0; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i]!='_') return 0; /* Check if the name contains only alphanumeric characters */
    }
    return 1; /* Valid name */
}

/* Function that creates the table of macros, initially it's empty. */
MacroTable* create_macro_table() {
    MacroTable *macro_table;
    /*Allocate memory for the newly created table*/
    macro_table = (MacroTable *)malloc(sizeof(MacroTable));
    if (macro_table) {
        macro_table->size = 0;
        macro_table->table = NULL;
    }
    return macro_table;
}

/*Function that copies the content of a string into another string*/
char *copy(const char *s) {
    char *d = malloc(strlen(s) + 1);
    if (d == NULL) return NULL;
    strcpy(d, s);
    return d;
}

/* Function that inserts a macro into the macro table*/
void insert_macro(MacroTable *macro_table, Macro *macro) {
    MacroNode *new_node;

    /* Check if the macro table has reached its maximum size */
    if (macro_table->size >= MAX_NUM_OF_MACROS) {
        fprintf(stderr, "Error: Macro table is full. Cannot insert new macro: %s\n", macro->name);
        return;
    }
    /*Allocating memory for the new macro in the macro table*/
    if (macro_table->size == 0) {
        macro_table->table = (MacroNode **)malloc(sizeof(MacroNode *));
    } else {
        MacroNode **new_table = (MacroNode **)realloc(macro_table->table, (macro_table->size + 1) * sizeof(MacroNode *));
        if (new_table) {
            macro_table->table = new_table;
        } else {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return;
        }
    }
    /*Allocating memory for the new Macro node, giving it its value, and adding it to the designed place in the table*/
    new_node = (MacroNode *)malloc(sizeof(MacroNode));
    if (new_node) {
        new_node->name = copy(macro->name);
        new_node->macro = macro;
        new_node->next = NULL;
        macro_table->table[macro_table->size] = new_node;
        macro_table->size++;
    } else {
        fprintf(stderr, "Error: Memory allocation failed for new macro node.\n");
    }
}
/* Function that finds a macro inside the macro table*/
Macro* find_macro(MacroTable *macro_table, const char *name) {
    int i;
    MacroNode *node;
    for (i = 0; i < macro_table->size; ++i) {
        node = macro_table->table[i];
        /*Compare the given macro name with all macro names in the table*/
        if (strcmp(node->name, name) == 0) {
            return node->macro;
        }
    }
    return NULL;
}

/*This function creates a new macro with the given name, as a linked list of lines*/
Macro* create_macro(const char *name) {
    Macro *macro;
    macro = (Macro *)malloc(sizeof(Macro));
    if (macro) {
        macro->name = copy(name);
        macro->lines = NULL;  /* Initialize the head of the linked list to NULL */
    }
    return macro;
}

/* Adds a line to an existing macro struct.*/
void add_macro_line(Macro *macro, const char *line, int line_number) {
    Line *new_line;
    Line *current;
    /*ALlocate memory for the new line*/
    new_line = (Line *)malloc(sizeof(Line));
    if (new_line) {
        new_line->value = copy(line);
        new_line->number = line_number;
        new_line->next = NULL;  /* New line will be the last one, so its next is NULL */

        if (macro->lines == NULL) {
            /* If the list is empty, the new line is the head */
            macro->lines = new_line;
        } else {
            /* Otherwise, find the end of the list and append the new line */
            current = macro->lines;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_line;
        }
    }
}

/* Frees the memory allocated for a macro. */
void free_macro(Macro *macro) {
    Line *current;
    Line *next;
    current = macro->lines;
    while (current) {
        next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
    free(macro->name);
    free(macro);
}

/* Read a line from a file and trim whitespace */
char* read_line(FILE *file, char *buffer, int size, int *line_number) {
    char *start;
    char *end;
    /*Read the line into buffer*/
    if (fgets(buffer, size, file) == NULL) {
        return NULL;
    }

    (*line_number)++;

    /* Delete whitespaces at the start of the string by moving the start pointer to the right */
    start = buffer;
    while (*start == ' ' || *start == '\t') start++;

    /* Delete whitespaces at the end of the string by moving the end pointer to the left*/
    end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = '\0';

    return start;
}

 /*The function validates the correctnes of the macro definition and saves it in the macro table*/
int process_macro(MacroTable *macro_table, char *line, char *macro_name, int line_number) {
    Macro *macro;
    strcpy(macro_name, line + MACRODEF);  /* Save the name of the macro, skipping the "macr " part */
    if (!validate_macro(macro_name, macro_table)) {
        fprintf(stderr, "Invalid macro name: %s (line %d)\n", macro_name, line_number);
        return 0; /*Macro name is not valid, exit to the next file*/
    }
    macro = create_macro(macro_name);
    insert_macro(macro_table, macro);
    return 1;
}

/* This functions adds data to a given macro, by finding it and adding to it a line.*/
void macro_data(MacroTable *macro_table, const char *macro_name, char *line, int line_number) {
    Macro *macro;
    macro = find_macro(macro_table, macro_name);
    if (macro) {
        add_macro_line(macro, line, line_number);
    }
}

/* Expand macros in the input file */
void expand_macros(MacroTable *macro_table, char *line, FILE *out) {
    char* macro_name;
    Macro *macro;
    Line *current;
    macro_name = line;
    /*Check whether it's a defined macro*/
    macro = find_macro(macro_table, macro_name);
    if (macro) {
        /*If it's a macro, write all the lines inside the macro to the file*/
        current = macro->lines;
        while (current) {
            fputs(current->value, out);
            fputc('\n', out);
            current = current->next;
        }
    } else {
        /*Otherwise write the line as it is to the file*/
        fputs(line, out);
        fputc('\n', out);
    }
}





/*This function is the main function, it parses the macros line by line
 * categorizing each line (either a macro definition, macro end, inside a macro or outside a macro*/
int parse_macros(const char *input_file, const char *output_file, MacroTable *macro_table) {
    FILE *in;
    FILE *out;
    int in_macro;
    int line_number;
    char line[BIG_LEN];
    char macro_name[BIG_LEN];
    char* beg;

    /* Open the input and output files */
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    if (!in || !out) {
        fprintf(stderr, "Error opening file.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        /* Delete the file if there was a problem */
        remove(output_file);
        return 0;
    }

    /* Create an empty macro table */
    /* In macro is a flag that tells us if we are inside a macro right now */
    in_macro = 0;
    line_number = 0;
    /* Reading the file lines one by one, and processing them each to the correct category */
    while (read_line(in, line, sizeof(line), &line_number)) {

        /* Macro definition case, we ignore spaces at the beginning */
        beg = line;
        while(*beg == ' ') beg++;
        if (strncmp(beg, "macr ", MACRODEF) == 0 && !in_macro) {
            if (!process_macro(macro_table, beg, macro_name, line_number)) {
                fclose(in);
                fclose(out);
                remove(output_file);
                return 0;
            }
            in_macro = 1;
        } else if (strncmp(beg, "endmacr", MACEND) == 0 && in_macro) {
            /* Macro ending case */
            if (strlen(line) > MACEND) {
                /* We have chars after the endmacr, resulting in error, so we delete the file in this case */
                fprintf(stderr, "Extra characters after endmacr: %s (line %d)\n", beg, line_number);
                fclose(in);
                fclose(out);
                remove(output_file);
                return 0;
            }
            in_macro = 0;
        } else if (in_macro) {
            /* We are inside a macro case, then we save the data to current macro we are inside of */
            macro_data(macro_table, macro_name, beg, line_number);
        } else {
            /* Outside of a macro case, then we need to add the line to the output file
             * or expand the macro if it's a macro name
             */
            expand_macros(macro_table, beg, out);
        }
    }

    fclose(in);
    fclose(out);

    return 1;
}

/*This functin frees the macro table given as input*/
void free_macro_table(MacroTable *macro_table) {
    int i;
    for (i = 0; i < macro_table->size; i++) {
        MacroNode *node = macro_table->table[i];
        while (node) {
            MacroNode *next = node->next;
            free_macro(node->macro);
            free(node->name);
            free(node);
            node = next;
        }
    }
    free(macro_table->table);
    free(macro_table);
}

