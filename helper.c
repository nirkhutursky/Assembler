
#include "helper.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "AssemblyConstants.h"


/* This functions adds .as ending to a given file */
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
            /*End the buffer and convert it to int*/
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
        for (i = 0; i < cnt; i++) {
            /*Checking that the number is in the range*/
            if (dataArr[i]>DATA_RANGE || dataArr[i]<(-DATA_RANGE)) {
                prer(lineNum, "Number not in the allowed range");
                return ERR;
            }
        }
        return cnt;
    }

    if (strcmp(instruction,".string")==0) {
        strArr = parse_word(remainder);
        if (strArr==NULL) {
            prer(lineNum, "Invalid input for .string instruction");
        }
        /*Including the end of line character*/
        return strlen(strArr)+1;
    }
    /*.extern and .entry don't add to data counter and a label before them is ignored, therefore we don't increase the IC in this case*/
    return 0;
}


int get_operand_type(char *oper, int lineNum) {
    int i, num=0;
    if (oper==NULL) return NONE;
    if (oper[0]=='#') {
        if (strlen(oper)>BASE) {
            prer(lineNum, "Number is invalid, too long");
            return ERR;
        }
        for (i=1; i<strlen(oper); i++) {
            if (i==1 && oper[i]=='+' || oper[i]=='-') continue;
            if (!isalnum(oper[i])) {
                prer(lineNum, "Invalid immediate adressing, number is not defined");
                prer(lineNum, "Invalid immediate adressing, number is not defined");
                return ERR;
            }
            num*=BASE;
            num+=(oper[i]-'0');
        }
        if (num>RANGE || num <(-RANGE)) {
            prer(lineNum, "Number is invalid (Too long or undefined)");
            return ERR;
        }
        return IMME;
    }
    if (strlen(oper)==DIR_REG_LEN && oper[0]=='r' && oper[1]<='7' && oper[1]>='0') return DIR_REG;
    if (strlen(oper)==UNDIR_REG_LEN && oper[0]=='*' && oper[1]=='r' && oper[2]<='7' && oper[2]>='0') return UNDIR_REG;
    return DIR;
}

int calc_IC(int type1, int type2) {
    int add = 1;
    if (type1==IMME || type1==DIR) add++;
    if (type2==IMME || type2==DIR) add++;
    if (type1==DIR_REG || type1==UNDIR_REG) add++;
    else if (type2==DIR_REG || type2==UNDIR_REG) add++;
    return add;
}


void DC_mem_calc(LabelTable *label_table, int IC) {
    int i;
    for (i = 0; i < label_table->count; i++) {
        if (label_table->label_list[i].type == 1) {
            label_table->label_list[i].address += IC;
        }
    }
}



int valid_oper_oper(int op1,int op2, char *operation, int lineNum, int op_count){
	if (strcmp(operation,"mov")==0){
		if (op_count!=T_OP){
	        prer(lineNum, "Number of operands for mov is incorrect");
	        return 0;
		}
	if (op2==IMME) {
	    prer(lineNum, "Addressing for mov operands is incorrect");
	    return 0;
	}
	    return 1;
    }

    if (strcmp(operation,"cmp")==0){
	    if (op_count!=T_OP){
	        prer(lineNum, "Number of operands for cmp is incorrect");
	        return 0;
	    }
        return 1;
    }

    if (strcmp(operation,"add")==0){
	if (op_count!=T_OP){
	prer(lineNum, "Number of operands for add is incorrect");
	return 0;
    }
	if (op2==IMME){
	prer(lineNum, "Addressing of add operands is incorrect");
	return 0;
    }
    return 1;
    }

    if (strcmp(operation,"sub")==0){
    if (op_count!=T_OP){
	    prer(lineNum, "Number of operands for sub is incorrect");
	    return 0;
    }
	if (op2==IMME){
	    prer(lineNum, "Addressing of sub operands is incorrect");
    	return 0;
    }
    return 1;
    }

    if (strcmp(operation,"lea")==0){
	if (op_count!=T_OP){
	    prer(lineNum, "Number of operands for lea is incorrect");
	    return 0;
	}
	if (op2==IMME){
	prer(lineNum, "Addressing of lea operands is incorrect");
	return 0;
    }
    return 1;
    }

    if (strcmp(operation,"clr")==0){
	    if (op_count!=O_OP){
	    prer(lineNum, "Number of operands for clr is incorrect");
	    return 0;
    }
	if (op2!=NONE || op1==IMME || op1 == NONE){
	    prer(lineNum, "Addressing of clr operands is incorrect");
	    return 0;
	}
        return 1;
    }

    if (strcmp(operation,"not")==0){
    	if (op_count!=O_OP){
	    prer(lineNum, "Number of operands for not is incorrect");
	    return 0;
    }
	if (op2!=NONE || op1==IMME || op1 == NONE){
	prer(lineNum, "Addressing of not operands is incorrect");
	return 0;
    }
    return 1;
    }

    if (strcmp(operation,"inc")==0){
	    if (op_count!=O_OP){
	    prer(lineNum, "Number of operands for inc is incorrect");
    	return 0;
    }
	if (op2!=NONE || op1==IMME || op1 == NONE){
	prer(lineNum, "Addressing of inc operands is incorrect");
	return 0;
    }
    return 1;
    }

    if (strcmp(operation,"dec")==0){
	    if (op_count!=O_OP){
	    prer(lineNum, "Number of operands for dec is incorrect");
	    return 0;
    }
	if (op2!=NONE || op1==IMME || op1 == NONE){
	prer(lineNum, "Addressing of dec operands is incorrect");
	return 0;
    }
    return 1;
    }

    if (strcmp(operation,"jmp")==0){
	    if (op_count!=O_OP){
    	prer(lineNum, "Number of operands for jmp is incorrect");
	return 0;
    }
	if (op2!=NONE || op1==IMME || op1==DIR_REG || op1 == NONE){
	prer(lineNum, "Addressing of jmp operands is incorrect");
	return 0;
    }
        return 1;
    }

    if (strcmp(operation,"bne")==0){
	    if (op_count!=O_OP){
	    prer(lineNum, "Number of operands for bne is incorrect");
	    return 0;
	    }
	if (op2!=NONE || op1==IMME || op1==DIR_REG || op1 == NONE){
	prer(lineNum, "Addressing of bne operands is incorrect");
	return 0;
    }
    return 1;
    }
if (strcmp(operation,"red")==0){
	if (op_count!=O_OP){
	prer(lineNum, "Number of operands for red is incorrect");
	return 0;
    }
	if (op2!=NONE || op1==IMME || op1 == NONE){
	prer(lineNum, "Addressing of red operands is incorrect");
	return 0;
    }
    return 1;
    }
if (strcmp(operation,"prn")==0){
	if (op_count!=O_OP){
	prer(lineNum, "Number of operands for prn is incorrect");
	return 0;
    }
	if (op2!=NONE || op1 == NONE){
	prer(lineNum, "Addressing of prn operands is incorrect");
	return 0;
    }
    return 1;
    }


if (strcmp(operation,"jsr")==0){
	if (op_count!=O_OP){
	prer(lineNum, "Number of operands for jsr is incorrect");
	return 0;
    }
	if (op2!=NONE || op1==IMME || op1==DIR_REG || op1 == NONE){
	prer(lineNum, "Addressing of jsr operands is incorrect");
	return 0;
    }
    return 1;
    }
if (strcmp(operation,"rts")==0){
	if (op_count!=Z_OP){
	prer(lineNum, "Number of operands for rts is incorrect");
	return 0;
	}
    return 1;
    }
if (strcmp(operation,"stop")==0){
	if (op_count!=Z_OP){
	prer(lineNum, "Number of operands for stop is incorrect");
	return 0;
    }
    return 1;
    }
prer(lineNum, "Operation is not defined");
return 0;
}


signed int getNumber(char *op) {
    /*First char is '#' so we ignore it */
    signed int num, sign, i;
    sign = 1;

    op++;
    if (op[0]=='-') {
        sign = -1;
        op++;
    }
    else if (op[0]=='+') op++;
    num = 0;
    for (i=0; i<strlen(op); i++) {
        num*=10;
        num+=(op[i]-'0');
    }
    /*Multyplying by -1 in case of - in the start*/
    return num*sign;

}
