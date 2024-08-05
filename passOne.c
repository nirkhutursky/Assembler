#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AssemblyConstants.h"
#include "helper.h"
#include "MacroProcessing.h"
#include "structs.h"





int print_labels_content(const char *filename, MacroTable *macro_table) {
    FILE *file;
    char line[2345];
    char *remainder;
    char *label = NULL;
    char *instruction = NULL;
    int cnt;
    int *dataArr;
    int lineNum = 0;
    int i=0;
    int ErrorFlag = 1;
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s could not be opened\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;
        /* Check if the line is truncated */
        if (strlen(line)>LINE_SIZE) {
            prer(lineNum, "Line is too long");
            ErrorFlag = 0;
            continue;
        }
        /*Skip the line if it's a comment or an empty line; we already deleted all the trailing spaces*/
        if (line[0]==';' || line[0]=='\n') continue;

        /* Process the line */
        remainder = get_line_remainder(line, &label, &instruction);
        if (!validate_line(line,label,instruction,remainder,lineNum,macro_table)) {
            ErrorFlag = 0;
            continue;
        }

        if (strcmp(instruction,".data")==0) {
            dataArr = parse_numbers(remainder, &cnt);
            if (dataArr==NULL) {
                prer(lineNum, "Invalid input for .data instruction");
                ErrorFlag = 0;
                continue;
            }

        }


        free(remainder);
        free(label);
        free(instruction);
    }
    printf("%d\n", ErrorFlag);

    fclose(file);
    return ErrorFlag;
}




int validate_line(char *line, char *label, char *instruction, char *remainder, int lineNum,MacroTable *macro_table) {
    int i;

    if (remainder==NULL) {
        prer(lineNum, "Space after label name");
        return 0;
    }
    if (!valid_label(label,macro_table)) {
        prer(lineNum,"Label name is invalid");
    }
    if (!valid_instruction(instruction)) {
        prer(lineNum,"Instruction name is invalid");
        return 0;
    }

    /*printf("%s x %s x %s x\n",label, instruction, remainder);*/
    return 1;
}

int valid_label(char *label, MacroTable *macro_table) {
    int i;
    if (label==NULL) return 1;/*No label*/
    /* Check that it's not an operation name */
    for (i = 0; i < NUM_OF_OPERATIONS; ++i) {
        if (strcmp(label, operations[i]) == 0) {
            return 0; /* Invalid name */
        }
    }

    /* Check that it's not an instruction names */
    for (i = 0; i < NUM_OF_INSTRUCTIONS; ++i) {
        if (strcmp(label, instructions[i]) == 0) {
            return 0; /* Invalid name */
        }
    }
    for (i = 0; i < NUM_OF_REGISTERS; ++i) {
        if (strcmp(label, registers[i]) == 0) {
            return 0; /* Invalid name */
        }
    }

    /*Check that it's not an a macro name*/
    if (find_macro(macro_table, label) != NULL) return 0;
    /*Label is too long*/
    if (strlen(label)>LABEL_SIZE) return 0;
    /*Check if the label is not according to the needed format*/
    if (!isalpha(label[0])) return 0;
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[i]) && label[i]!='_') return 0; /* Check if the name contains only alphanumeric characters */
    }
    return 1; /* Valid name */
}

int valid_instruction(char *instruction) {
    int value = 0;
    int i;
    for (i = 0; i < NUM_OF_INSTRUCTIONS; ++i) {
        if (strcmp(instruction, instructions[i]) == 0) {
            /*Assigning the correct value to the instruction*/
            value = 1; /* Valid name */
        }
    }
    for (i = 0; i < NUM_OF_OPERATIONS; ++i) {
        if (strcmp(instruction, operations[i]) == 0) {
            /*Index moved by constant (num of instructions)*/
            value = 1;
        }
    }
    return value;
}




char* process_label(const char *line, char **label) {
    const char *col_pos;
    int label_length;
    char *remaining_line;
    int i, j;

    col_pos = strchr(line, ':');

    if (col_pos == NULL) {
        *label = NULL;
        remaining_line = (char *)malloc(strlen(line) + 1);
        if (remaining_line) {
            strcpy(remaining_line, line);
        }
        return remaining_line; /* No col found, return a copy of the whole line */
    }

    /* Calculate length of the potential label */
    label_length = col_pos - line;

    /* Check for spaces before the col */
    for (i = 0; i < label_length; i++) {
        if (isspace(line[i])) {
            *label = NULL;
            return NULL; /* Invalid label due to space before the col */
        }
    }

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
    remaining_line = (char *)malloc(strlen(col_pos + 1) + 1); /* Skip the col */
    if (remaining_line) {
        strcpy(remaining_line, col_pos + 1);
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
    if (line_after_label==NULL) return NULL;
    free(line);

    /* process the instruction */
    remainder = process_instruction(line_after_label, instruction);
    free(line_after_label);

    /* process trailing spaces from the remainder */
    process_trailing_spaces(remainder);

    return remainder;
}






