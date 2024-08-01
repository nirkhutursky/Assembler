
#include "helper.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AssemblyConstants.h"


/* This functions adds .as ending to a given filer */
void get_out_name( char *input_filename, char *output_filename) {
    strcpy(output_filename, input_filename);
    strcat(input_filename, ".as");
    strcat(output_filename, ".am");
}

int is_label(const char *line, char **label) {
    const char *colon_pos;
    int label_length;
    int i;

    colon_pos = strchr(line, ':');

    if (colon_pos == NULL) {
        return 0; /* No colon found, not a label */
    }

    /* Trim spaces before the colon */
    while (colon_pos > line && isspace(*(colon_pos - 1))) {
        colon_pos--;
    }

    /* Calculate length of the potential label */
    label_length = colon_pos - line;
    if (label_length > LABEL_SIZE) {
        return 0; /* Label is too long */
    }

    /* Check if the potential label is valid */
    for (i = 0; i < label_length; i++) {
        if (!isalnum(line[i])) {
            return 0; /* Invalid character in label */
        }
    }

    /* Allocate memory and copy the label */
    *label = (char *)malloc(label_length + 1);
    if (*label == NULL) {
        return 0; /* Memory allocation failed */
    }
    strncpy(*label, line, label_length);
    (*label)[label_length] = '\0';

    return 1;
}

int is_instruction(const char *line) {
    char *start;
    char *label = NULL;
    int result = 0;

    /* Check if the line starts with any of the instruction types */
    if (strncmp(line, ".data", 5) == 0 ||
        strncmp(line, ".string", 7) == 0 ||
        strncmp(line, ".entry", 6) == 0 ||
        strncmp(line, ".extern", 7) == 0) {
        result = 1;
        } else if (is_label(line, &label)) {
            /* Move past the label to the rest of the line */
            start = (char *)(line + strlen(label) + 1);
            while (*start == ' ' || *start == '\t') start++;  /* Skip spaces after the label */

            /* Check if the remaining part of the line starts with any of the instruction types */
            if (strncmp(start, ".data", 5) == 0 ||
                strncmp(start, ".string", 7) == 0 ||
                strncmp(start, ".entry", 6) == 0 ||
                strncmp(start, ".extern", 7) == 0) {
                result = 1;
                }
        }

    if (label) free(label);
    return result;
}

void prer(int lineNum, char* error_type) {
    printf("Error: %s! The error is on line %d\n", error_type, lineNum);
}