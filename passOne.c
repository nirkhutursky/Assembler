#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AssemblyConstants.h"
#include "helper.h"
#include "MacroProcessing.h"
#include "structs.h"







int print_labels_content(const char *filename) {
    FILE *file;
    char line[2345];
    char *remainder;
    char *label = NULL;
    char *instruction = NULL;
    int lineNum = 0;
    int len;
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;

        /* Check if the line is truncated */
        len = strlen(line);
        if (strlen(line)>LINE_SIZE) {
            prer(lineNum, "Line too long");
            continue;
        }

        /* Process the line */
        remainder = get_line_remainder(line, &label, &instruction);
        printf("Line - Label: %s, Instruction: %s, Remainder: %s", label , instruction , remainder);
        free(remainder);
        free(label);
        free(instruction);
    }

    fclose(file);
    return 1;
}


char* trim_label(const char *line, char **label) {
    const char *colon_pos;
    int label_length;
    char *remaining_line;

    colon_pos = strchr(line, ':');

    if (colon_pos == NULL) {
        *label = NULL;
        remaining_line = (char *)malloc(strlen(line) + 1);
        if (remaining_line) {
            strcpy(remaining_line, line);
        }
        return remaining_line; /* No colon found, return a copy of the whole line */
    }

    /* Trim spaces before the colon */
    while (colon_pos > line && isspace(*(colon_pos - 1))) {
        colon_pos--;
    }

    /* Calculate length of the potential label */
    label_length = colon_pos - line;

    /* Allocate memory and copy the label */
    *label = (char *)malloc(label_length + 1);
    if (*label == NULL) {
        return NULL; /* Memory allocation failed */
    }
    strncpy(*label, line, label_length);
    (*label)[label_length] = '\0';

    /* Allocate memory and copy the remaining part of the line */
    remaining_line = (char *)malloc(strlen(colon_pos + 1) + 1); /* Skip the colon */
    if (remaining_line) {
        strcpy(remaining_line, colon_pos + 1);
    }
    return remaining_line;
}

/*
 * Function: trim_instruction
 * --------------------------
 * Trims the instruction from the given line and returns the remaining line.
 *
 * Parameters:
 *  - line: The line to process.
 *  - instruction: Pointer to store the dynamically allocated instruction if found.
 *
 * Returns:
 *  - Pointer to the remaining part of the line after the instruction.
 */
char* trim_instruction(const char *line, char **instruction) {
    const char *start = line;
    const char *end;
    char *remaining_line;
    int instruction_length;

    /* Skip leading spaces */
    while (*start == ' ' || *start == '\t') start++;

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
char* get_line_remainder(char *line, char **label, char **instruction) {
    char *remainder = NULL;

    /* Trim the label */
    char *line_after_label = trim_label(line, label);
    free(line);

    /* Trim the instruction */
    remainder = trim_instruction(line_after_label, instruction);
    free(line_after_label);

    return remainder;
}




