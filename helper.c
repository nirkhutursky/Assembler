
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



void prer(int lineNum, char* error_type) {
    printf("Error on line %d: %s\n", lineNum, error_type);
}




int count_numbers(const char *line) {
    int count = 0;
    char *ind = line;
    int in_number = 0;

    while (*ind != '\0') {
        if (isdigit(*ind) || (*ind == '+' || *ind == '-' && isdigit(*(ind + 1)))) {
            if (!in_number) {
                count++;
                in_number = 1;
            }
        } else if (*ind == ',') {
            in_number = 0;
        }
        ind++;
    }

    return count;
}

/* Function to validate and parse the input string into an array of integers */
int* parse_numbers(const char *line, int *num_count) {
    const char *ind = line;
    int number_started = 0;
    int comma_expected = 0;
    int count = count_numbers(line);
    int *numbers = (int *)malloc(count * sizeof(int));
    int index = 0;
    char buffer[20]; /* Buffer to store each number as a string */
    int buffer_index = 0;

    if (numbers == NULL) {
        *num_count = 0;
        return NULL; /* Memory allocation failed */
    }

    while (*ind != '\0') {
        if (*ind == ' ' || *ind == '\t') {
            /*We ignore all the spaces before and after commas*/
            ind++;
        } else if (*ind == '+' || *ind == '-') {
            /*Make sure that we don't have a sign in the middle of a number*/
            if (number_started || comma_expected) {
                free(numbers);
                *num_count = 0;
                return NULL;
            }
            number_started = 1;
            buffer[buffer_index++] = *ind;
            ind++;
        } else if (isdigit(*ind)) {
            number_started = 1;
            comma_expected = 1;
            buffer[buffer_index++] = *ind;
            ind++;
        } else if (*ind == ',') {
            /*Commas may appear only after a number*/
            if (!comma_expected) {
                free(numbers);
                *num_count = 0;
                return NULL;
            }
            /*End the buffer and convert it to string*/
            buffer[buffer_index] = '\0';
            numbers[index++] = atoi(buffer);
            buffer_index = 0;
            /* Nullify the flags */
            number_started = 0;
            comma_expected = 0;
            ind++;
        } else {
            /*Invalid character*/
            free(numbers);
            *num_count = 0;
            return NULL;
        }
    }

    /* Adding the last number to the list*/
    if (number_started) {
        buffer[buffer_index] = '\0';
        numbers[index++] = atoi(buffer);
    } else {
        /*Last number is invalid*/
        free(numbers);
        *num_count = 0;
        return NULL;
    }
    /*Saving the ammount of numbers*/
    *num_count = count;
    return numbers;
}


void instruction_tree(char* ins) {

    return;
}