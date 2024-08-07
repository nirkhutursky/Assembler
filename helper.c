
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
int empty_s(const char *str) {
    while (*str) {
        if (*str != ' ' && *str != '\t' && *str != '\n') {
            return 0;  /*String is not empty*/
        }
        str++;
    }
    return 1;  /*Empty string*/
}

int count_commas(const char *str) {
    int count = 0;

    while (*str) {
        if (*str == ',') {
            count++;
        }
        str++;
    }

    return count;
}

int parse_operands(const char *line, char **operand1, char **operand2, int lineNum) {
    const char *delimiter = ",";
    char line_copy[LINE_SIZE];
    char *oper;
    /*Eventually we return the number of operands (that are non null)*/
    int count = 0;
    char *end;

    if (count_commas(line)>=MAX_OPES) {
        prer(lineNum, "Too many operands");
        return ERR;
    }

    /*The operands are null be default, because we may have less than 2 operands*/
    *operand1 = NULL;
    *operand2 = NULL;
    if (empty_s(line)) return 0;

    strncpy(line_copy, line, LINE_SIZE - 1);
    line_copy[LINE_SIZE - 1] = '\0';

    /*Getting the value of the first operand*/
    oper = strtok(line_copy, delimiter);
    if (oper != NULL) {
        /*Deleting not needed spaces*/
        while (isspace(*oper)) oper++;
        end = oper + strlen(oper) - 1;
        while (end > oper && isspace(*end)) end--;
        end[1] = '\0';
        /*Allocating memory for the operand*/
        *operand1 = (char *)malloc(strlen(oper) + 1);
        if (*operand1 == NULL) {
            fprintf(stderr, "Memory allocation failure\n");
            exit(1);
        }
        strcpy(*operand1, oper);
        count++;
    }
    /*Getting the value of the second operand*/
    oper = strtok(NULL, delimiter);
    if (oper != NULL) {
        while (isspace(*oper)) oper++;
        end = oper + strlen(oper) - 1;
        while (end > oper && isspace(*end)) end--;
        end[1] = '\0';

        *operand2 = (char *)malloc(strlen(oper) + 1);
        if (*operand2 == NULL) {
            fprintf(stderr, "Memory allocation failure\n");
            free(*operand1);
            exit(1);
        }
        strcpy(*operand2, oper);
        count++;
    }

    return count;
}



/* Function to parse a word from a string, the word is started and ended by a " (which should be ignored) */
char* parse_word(const char *line) {
    int len;
    char *output;
    len = strlen(line);
    /*The string is too short or is NULL*/
    if (line == NULL || len < 2) {
        return NULL;
    }



    /* Checking if the string starts and ends with quotes*/
    if (line[0] != '"' || line[len - 1] != '"') {
        return NULL;
    }


    output = (char*)malloc(len - 1);
    if (output == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    /* We delete the quotes so now the lenegth of the resulting string is shorter by 2 */
    len--;
    len--;


    /*Copying the string into output besides the quotes*/
    strncpy(output, line + 1, len);
    output[len] = '\0';

    return output;
}


int count_special_instruction(char *instruction, char *remainder, int lineNum) {
    int cnt, *dataArr;
    char *strArr;
    int i;
    if (strcmp(instruction,".data")==0) {
        dataArr = parse_numbers(remainder, &cnt);

        if (dataArr==NULL) {
            prer(lineNum, "Invalid input for .data instruction");
            return ERR;
        }
        /*for (i = 0; i < cnt; i++) {
            printf("%d ", dataArr[i]);
        }
        printf("\n");*/
        return cnt;
    }

    if (strcmp(instruction,".string")==0) {
        strArr = parse_word(remainder);
        if (strArr==NULL) {
            prer(lineNum, "Invalid input for .string instruction");
        }
        printf("%s\n", strArr);
        return strlen(strArr);
    }
    /*.extern and .entry don't add data and a label before them is ignored, therefore we add 0 to IC in this case*/
    return 0;
}
