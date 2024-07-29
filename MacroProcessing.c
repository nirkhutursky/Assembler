#include "MacroProcessing.h"

#include <ctype.h>
#include "structs.h"
#include "AssemblyConstants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Function to check if a macro name is valid (not an operation or instruction name) */
int is_macro_name_valid(const char *name, MacroTable *macro_table) {
    int i;

    /* Check against operation names */
    for (i = 0; i < (int)(sizeof(operations) / sizeof(operations[0])); ++i) {
        if (strcmp(name, operations[i]) == 0) {
            return 0; /* Invalid name */
        }
    }

    /* Check against instruction names */
    for (i = 0; i < (int)(sizeof(instructions) / sizeof(instructions[0])); ++i) {
        if (strcmp(name, instructions[i]) == 0) {
            return 0; /* Invalid name */
        }
    }
    if (find_macro(macro_table, name) != NULL) return 0;

    for (i = 0; name[i] != '\0'; i++) {
        if (!isalnum(name[i])) return 0; /* Check if the name contains only alphanumeric characters */
    }
    return 1; /* Valid name */
}

/* Create a new macro table */
MacroTable* create_macro_table() {
    MacroTable *macro_table;
    macro_table = (MacroTable *)malloc(sizeof(MacroTable));
    if (macro_table) {
        macro_table->size = 0;
        macro_table->table = NULL;
    }
    return macro_table;
}

char *copy(const char *s) {
    char *d = malloc(strlen(s) + 1);
    if (d == NULL) return NULL;
    strcpy(d, s);
    return d;
}

/* Insert a macro into the macro table */
void insert_macro(MacroTable *macro_table, Macro *macro) {
    unsigned int index;
    MacroNode *new_node;

    /* Check if the macro table has reached its maximum size */
    if (macro_table->size >= MAX_NUM_OF_MACROS) {
        fprintf(stderr, "Error: Macro table is full. Cannot insert new macro: %s\n", macro->name);
        return;
    }

    if (macro_table->size == 0) {
        macro_table->table = (MacroNode **)malloc(sizeof(MacroNode *));
    } else {
        MacroNode **new_table = (MacroNode **)realloc(macro_table->table, (macro_table->size + 1) * sizeof(MacroNode *));
        if (new_table) {
            macro_table->table = new_table;
        } else {
            fprintf(stderr, "Error: Memory allocation failed while expanding macro table.\n");
            return;
        }
    }

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
/* Find a macro in the macro table */
Macro* find_macro(MacroTable *macro_table, const char *name) {
    int i;
    MacroNode *node;
    for (i = 0; i < macro_table->size; ++i) {
        node = macro_table->table[i];
        if (strcmp(node->name, name) == 0) {
            return node->macro;
        }
    }
    return NULL;
}

/* Create a new macro */
Macro* create_macro(const char *name) {
    Macro *macro;
    macro = (Macro *)malloc(sizeof(Macro));
    if (macro) {
        macro->name = copy(name);
        macro->lines = NULL;  /* Initialize the head of the linked list to NULL */
    }
    return macro;
}

/* Add a line to a macro */
void add_line_to_macro(Macro *macro, const char *line_value, int line_number) {
    Line *new_line;
    Line *current;
    new_line = (Line *)malloc(sizeof(Line));
    if (new_line) {
        new_line->value = copy(line_value);
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

/* Free a macro */
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
    if (fgets(buffer, size, file) == NULL) {
        return NULL;
    }

    (*line_number)++;

    /* Trim leading whitespace */
    start = buffer;
    while (*start == ' ' || *start == '\t') start++;

    /* Trim trailing whitespace */
    end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = '\0';

    return start;
}

char* process_line_ignoring_whitespace(char *line) {
    char *processed_line;
    processed_line = line + strspn(line, " \t");
    processed_line[strcspn(processed_line, "\n")] = '\0';
    return processed_line;
}

char* trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) {
        return str;
    }
    return str;
}

/* Handle the start of a macro definition */
int handle_macro_definition_start(MacroTable *macro_table, char *line, char *macro_name, int line_number) {
    Macro *macro;
    strcpy(macro_name, line + 5);  /* Copy the macro name, skipping the "macr " part */
    if (!is_macro_name_valid(macro_name, macro_table)) {
        fprintf(stderr, "Invalid macro name: %s (line %d)\n", macro_name, line_number);
        return 0; /* Exit on error */
    }
    macro = create_macro(macro_name);
    insert_macro(macro_table, macro);
    return 1;
}

/* Store a line in the macro definition */
void store_macro_definition(MacroTable *macro_table, const char *macro_name, char *line, int line_number) {
    Macro *macro;
    macro = find_macro(macro_table, macro_name);
    if (macro) {
        add_line_to_macro(macro, line, line_number);
    }
}

/* Expand macros in the input file */
void expand_macros(MacroTable *macro_table, char *line, FILE *out) {
    char macro_name[32];
    Macro *macro;
    Line *current;
    sscanf(line, "%s", macro_name);
    macro = find_macro(macro_table, macro_name);
    if (macro) {
        current = macro->lines;
        while (current) {
            fputs(current->value, out);
            fputc('\n', out);
            current = current->next;
        }
    } else {
        write_expanded_line(line, out);
    }
}

/* Write an expanded line to the output file */
void write_expanded_line(char *line, FILE *out) {
    fputs(line, out);
    fputc('\n', out); /* Ensure each line ends with a newline */
}

/* Main function to parse macros */
int parse_macros(const char *input_file, const char *output_file) {
    FILE *in;
    FILE *out;
    MacroTable *macro_table;
    int in_macro;
    int line_number;
    char line[LINE_SIZE + 1];
    char macro_name[32];
    int i;

    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    if (!in || !out) {
        fprintf(stderr, "Error opening file.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        remove(output_file);
        return 0;
    }

    macro_table = create_macro_table();
    in_macro = 0;
    line_number = 0;

    while (read_line(in, line, sizeof(line), &line_number)) {
        if (strncmp(line, "macr ", 5) == 0 && !in_macro) {
            if (!handle_macro_definition_start(macro_table, line, macro_name, line_number)) {
                fclose(in);
                fclose(out);
                remove(output_file);
                return 0;
            }
            in_macro = 1;
        } else if (strncmp(line, "endmacr", 7) == 0 && in_macro) {
            if (strlen(line) > 7) {
                fprintf(stderr, "Extra characters after endmacr: %s (line %d)\n", line, line_number);
                fclose(in);
                fclose(out);
                remove(output_file);
                return 0;
            }
            in_macro = 0;
        } else if (in_macro) {
            store_macro_definition(macro_table, macro_name, line, line_number);
        } else {
            expand_macros(macro_table, line, out);
        }
    }

    fclose(in);
    fclose(out);

    /* Free all macros in the macro table */
    for (i = 0; i < macro_table->size; i++) {
        MacroNode *node;
        node = macro_table->table[i];
        if (node) {
            free_macro(node->macro);
            free(node->name);
            free(node);
        }
    }
    free(macro_table->table);
    free(macro_table);
    return 1;
}
