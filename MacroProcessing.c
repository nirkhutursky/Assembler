#include "MacroProcessing.h"
#include "structs.h"
#include "AssemblyConstants.h"
#include <stdio.h>

/* Function to check if a macro name is valid (not an operation or instruction name) */
int is_macro_name_valid(const char *name,MacroTable *macro_table) {
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
    if (find_macro(macro_table, name)!=NULL) return 0;

    return 1; /* Valid name */
}

/* Create a new macro table */
MacroTable* create_macro_table() {
    MacroTable *macro_table;
    int i;
    macro_table = (MacroTable *)malloc(sizeof(MacroTable));
    if (macro_table) {
        for (i = 0; i < MAX_NUM_OF_MACROS; i++) {
            macro_table->table[i] = NULL;
        }
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
    index = hash(macro->name);
    new_node = (MacroNode *)malloc(sizeof(MacroNode));
    if (new_node) {
        new_node->name = copy(macro->name);
        new_node->macro = macro;
        new_node->next = macro_table->table[index];
        macro_table->table[index] = new_node;
    }
}

/* Find a macro in the macro table */
Macro* find_macro(MacroTable *macro_table, const char *name) {
    unsigned int index;
    MacroNode *node;
    index = hash(name);
    node = macro_table->table[index];
    while (node) {
        if (strcmp(node->name, name) == 0) {
            return node->macro;
        }
        node = node->next;
    }
    return NULL;
}

/* Create a new macro */
Macro* create_macro(const char *name) {
    Macro *macro;
    macro = (Macro *)malloc(sizeof(Macro));
    if (macro) {
        macro->name = copy(name);
        macro->lines = NULL;  /* Initialize the head of the linked list to NULL*/
    }
    return macro;
}

/* Add a line to a macro */
void add_line_to_macro(Macro *macro, const char *line_value, int line_number) {
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_line) {
        new_line->value = copy(line_value);
        new_line->number = line_number;
        new_line->next = NULL;  /* New line will be the last one, so its next is NULL*/

        if (macro->lines == NULL) {
            /* If the list is empty, the new line is the head*/
            macro->lines = new_line;
        } else {
            /* Otherwise, find the end of the list and append the new line*/
            Line *current = macro->lines;
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
    if (macro) {
        free(macro->name);
        current = macro->lines;
        while (current) {
            next = current->next;
            free(current->value);
            free(current);
            current = next;
        }
        free(macro);
    }
}

/* Hash function */
unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash % MAX_NUM_OF_MACROS;
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

/* Handle the start of a macro definition */
int handle_macro_definition_start(MacroTable *macro_table, char *line, char *macro_name, int line_number) {
    Macro *macro;
    sscanf(line + 4, "%s", macro_name);
    if (!is_macro_name_valid(macro_name,macro_table)) {
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
#define MAX_LINES_PER_MACRO 100  /* Maximum lines per macro for simplicity*/

void expand_macros(MacroTable *macro_table, char *line, FILE *out) {
    char macro_name[32];
    Macro *macro;
    Line *current;
    Line *lines[MAX_LINES_PER_MACRO];
    int line_count = 0;
    int i;
    sscanf(line, "%s", macro_name);
    macro = find_macro(macro_table, macro_name);
    if (macro) {
        current = macro->lines;
        /* Store lines in array*/
        while (current && line_count < MAX_LINES_PER_MACRO) {
            lines[line_count++] = current;
            current = current->next;
        }
        /* Write lines in correct order*/

        for ( i = 0; i < line_count ; i++) {
            fputs(lines[i]->value, out);
            fputc('\n', out);
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
    char line[LINE_SIZE + 1];
    char macro_name[32];
    int in_macro;
    int line_number;
    int i;
    printf("Input file: %s\n", input_file);
    printf("Output file: %s\n", output_file);
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    if (!in || !out) {
        fprintf(stderr, "Error opening file.\n");
        fclose(in);
        fclose(out);
        remove(output_file);
        return 0;
    }

    macro_table = create_macro_table();
    in_macro = 0;
    line_number = 0;
    while (read_line(in, line, sizeof(line), &line_number)) {
        /*printf("Current line: %s\n", line);*/
        if (strncmp(line, "macr ", 5) == 0 && !in_macro) {
            if (!handle_macro_definition_start(macro_table, line, macro_name, line_number)) {
                fclose(in);
                fclose(out);
                if (remove(output_file) == 0) {
                    printf("Deleted %s successfully.\n", output_file);
                } else {
                    perror("Error deleting file");
                }
                return 0;
            }
            in_macro = 1;
        } else if (strncmp(line, "endmacr", 7) == 0 && in_macro) {
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
    for (i = 0; i < MAX_NUM_OF_MACROS; i++) {
        MacroNode *node = macro_table->table[i];
        while (node) {
            MacroNode *next = node->next;
            free_macro(node->macro);
            free(node->name);
            free(node);
            node = next;
        }
    }
    free(macro_table);
    return 1;
}
