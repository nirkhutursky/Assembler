#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AssemblyConstants.h"
#include "helper.h"
#include "MacroProcessing.h"
#include "structs.h"







int validate_line(char *line, char *label, char *instruction, char *remainder) {
    return 1;
}

int valid_label(char *label, MacroTable *macro_table) {
    int i;
    /* Check that it's not an operation name */
    for (i = 0; i < (int)(sizeof(operations) / sizeof(operations[0])); ++i) {
        if (strcmp(label, operations[i]) == 0) {
            return 0; /* Invalid name */
        }
    }

    /* Check that it's not an instruction names */
    for (i = 0; i < (int)(sizeof(instructions) / sizeof(instructions[0])); ++i) {
        if (strcmp(label, instructions[i]) == 0) {
            return 0; /* Invalid name */
        }
    }

    /*Check that it's not an a macro name*/
    if (find_macro(macro_table, label) != NULL) return 0;
    /*Label is too long*/
    if (strlen(label)>LABEL_SIZE) return 0;
    /*The label is not according to the needed format*/
    if (!isalpha(label[0])) return 0;
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[i]) && label[i]!='_') return 0; /* Check if the name contains only alphanumeric characters */
    }
    return 1; /* Valid name */
}

int valid_instruction(char *instruction) {
    int flag = 0;
    int i=0;
    if (strcmp(instruction,".data")==0 || strcmp(instruction,".string")==0 || strcmp(instruction,".entry")==0 || strcmp(instruction,".extern")==0) flag = 2;
    for (i = 0; i < (int)(sizeof(operations) / sizeof(operations[0])); ++i) {
        if (strcmp(instruction, operations[i]) == 0) {
            flag = 1; /* Valid name */
        }
    }
    return flag;
}

int print_labels_content(const char *filename, MacroTable *macro_table) {
    FILE *file;
    char line[2345];
    char *remainder;
    char *label = NULL;
    char *instruction = NULL;
    int lineNum = 0;
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;
        /* Check if the line is truncated */
        if (strlen(line)>LINE_SIZE) {
            prer(lineNum, "Line is too long");
            continue;
        }
        /*Skip the line if it's a comment or an empty line; we already deleted all the trailing spaces*/
        if (line[0]==';' || line[0]=='\n') continue;

        /* Process the line */
        remainder = get_line_remainder(line, &label, &instruction);
        /*validate_line(line,label,instruction,remainder);*/
        if (instruction) printf("%s %d\n", instruction,  valid_instruction(instruction));
        free(remainder);
        free(label);
        free(instruction);
    }

    fclose(file);
    return 1;
}


char* process_label(const char *line, char **label) {
    const char *colon_pos;
    int label_length;
    char *remaining_line;
    int i, j;

    colon_pos = strchr(line, ':');

    if (colon_pos == NULL) {
        *label = NULL;
        remaining_line = (char *)malloc(strlen(line) + 1);
        if (remaining_line) {
            strcpy(remaining_line, line);
        }
        return remaining_line; /* No colon found, return a copy of the whole line */
    }

    /* Calculate length of the potential label */
    label_length = colon_pos - line;

    /* Allocate memory for the label, maximum possible length is label_length */
    *label = (char *)malloc(label_length + 1);
    if (*label == NULL) {
        return NULL; /* Memory allocation failed */
    }

    /* Copy the label without spaces */
    j = 0;
    for (i = 0; i < label_length; i++) {
        if (!isspace(line[i])) {
            (*label)[j++] = line[i];
        }
    }
    (*label)[j] = '\0';  /* Null-terminate the label */

    /* Allocate memory and copy the remaining part of the line */
    remaining_line = (char *)malloc(strlen(colon_pos + 1) + 1); /* Skip the colon */
    if (remaining_line) {
        strcpy(remaining_line, colon_pos + 1);
    }
    return remaining_line;
}
/*
 * Function: process_instruction
 * --------------------------
 * processs the instruction from the given line and returns the remaining line.
 *
 * Parameters:
 *  - line: The line to process.
 *  - instruction: Pointer to store the dynamically allocated instruction if found.
 *
 * Returns:
 *  - Pointer to the remaining part of the line after the instruction.
 */
char* process_instruction(const char *line, char **instruction) {
    const char *start = line;
    const char *end;
    char *remaining_line;
    int instruction_length;

    /* Skip leading spaces */
    while (*start == ' ' || *start == '\t' ) start++;

    /* Find the end of the instruction */
    end = start;
    while (!isspace((unsigned char)*end) && *end != '\0') {
        end++;
    }

    /* Calculate length of the instruction */
    instruction_length = end - start;

    /* Allocate memory and copy the instruction */
    *instruction = (char *)malloc(instruction_length + 1);
    if (*instruction == NULL) {
        return NULL; /* Memory allocation failed */
    }
    strncpy(*instruction, start, instruction_length);
    (*instruction)[instruction_length] = '\0';

    /* Skip spaces after the instruction */
    while (*end == ' ' || *end == '\t') end++;

    /* Allocate memory and copy the remaining part of the line */
    remaining_line = (char *)malloc(strlen(end) + 1);
    if (remaining_line) {
        strcpy(remaining_line, end);
    }
    return remaining_line;
}

/* Function to get the remainder of the line after label and instruction */
void process_trailing_spaces(char *str) {
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }
    *(end + 1) = '\0';
}

char* get_line_remainder(char *line, char **label, char **instruction) {
    char *remainder = NULL;

    /* process the label */
    char *line_after_label = process_label(line, label);
    free(line);

    /* process the instruction */
    remainder = process_instruction(line_after_label, instruction);
    free(line_after_label);

    /* process trailing spaces from the remainder */
    process_trailing_spaces(remainder);

    return remainder;
}




