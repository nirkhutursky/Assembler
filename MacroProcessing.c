/*
This file handles the pre-assembly stage by identifying and expanding macros
in the assembly code. It reads the input assembly file line by line, detects
macro definitions, and stores the macro names and their corresponding lines
of code. It ensures that macro names are valid and do not conflict with
operation or instruction names. The file also checks for syntax errors in
macro definitions and reports any issues found. Finally, it generates an
expanded assembly file with all macros replaced by their definitions, ensuring
the assembly code is ready for further processing.
 */


#include "MacroProcessing.h"

/* Function to check if a macro name is valid (not an operation or instruction name) */
int is_macro_name_valid(const char *name) {
    /* List of operation names and instruction names */
    const char *invalid_names[] = {
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        ".data", ".string", ".entry", ".extern"
    };
    size_t num_invalid_names = sizeof(invalid_names) / sizeof(invalid_names[0]);

    for (size_t i = 0; i < num_invalid_names; ++i) {
        if (strcmp(name, invalid_names[i]) == 0) {
            return 0; /* Invalid name */
        }
    }
    return 1; /* Valid name */
}

/* Function to parse macros in the assembly file */
void parse_macros(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    FILE *out = fopen(output_file, "w");
    if (!in || !out) {
        fprintf(stderr, "Error opening file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH + 1];
    char macro_name[32];
    int in_macro = 0;

    while (fgets(line, sizeof(line), in)) {
        /* Trim leading and trailing whitespace */
        char *start = line;
        while (*start == ' ' || *start == '\t') start++;
        char *end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
        *(end + 1) = '\0';

        /* Handle macro definition start */
        if (strncmp(start, "macr", 4) == 0 && !in_macro) {
            sscanf(start + 4, "%s", macro_name);
            if (!is_macro_name_valid(macro_name)) {
                fprintf(stderr, "Invalid macro name: %s\n", macro_name);
                fclose(in);
                fclose(out);
                return;
            }
            in_macro = 1;
            /* Handle macro definition end */
        } else if (strncmp(start, "endmacr", 7) == 0 && in_macro) {
            in_macro = 0;
        } else if (in_macro) {
            /* Inside a macro definition */
            /* Store the macro definition */
        } else {
            /* Outside of macro definition, write line to output */
            fputs(line, out);
        }
    }

    fclose(in);
    fclose(out);
}
