#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AssemblyConstants.h"
#include "helper.h"
#include "MacroProcessing.h"
#include "structs.h"





int pass_one(const char *filename, MacroTable *macro_table, LabelTable *label_table, int *DC) {
    FILE *file;
    char line[LINE_SIZE], *remainder, *label = NULL, *instruction = NULL, *op1, *op2,type1,type2;


    int lineNum = 0, op_count,  IC = ADDRESS_START, ErrorFlag = 1, add;

    *DC=0;
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s could not be opened\n", filename);
        return 0;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;
        /*Skip the line if it's a comment or an empty line; we already deleted all the starting spaces*/
        if (line[0]==';' || line[0]=='\n') continue;
        /* Process the line */
        remainder = get_line_remainder(line, &label, &instruction);
        if (!validate_line(line,label,instruction,remainder,lineNum,macro_table,label_table,IC,*DC)) {
            ErrorFlag = 0;
            continue;
        }


        if (instruction && instruction[0]=='.') {
            add = count_special_instruction(instruction, remainder, lineNum);
            if (add==ERR) {
                ErrorFlag = 0;
                continue;
            }
            *DC+=add;
            continue;
        }

        op_count = parse_operands(remainder,&op1,&op2);
        if (op_count==ERR) {
            prer(lineNum, "Too many operands");
            ErrorFlag = 0;
        }
        type1 = get_operand_type(op1, lineNum);
        type2 = get_operand_type(op2, lineNum);
        /*
        printf("%s %s %s\n",instruction, op1, op2);
        */
        IC+=calc_IC(type1,type2);








        free(remainder);
        free(label);
        free(instruction);
    }

    DC_mem_calc(label_table, IC);
    /*Passing the final value of IC to DC, as the data and string will start from this index in the machine code*/
    *DC = IC;
    fclose(file);
    return ErrorFlag;
}




int validate_line(char *line, char *label, char *instruction, char *remainder, int lineNum,MacroTable *macro_table, LabelTable *label_table, int IC, int DC) {


    if (remainder==NULL) {
        prer(lineNum, "Space after label name");
        return 0;
    }
    if (strlen(line)>LINE_SIZE) {
        prer(lineNum, "Line is too long");
        return 0;
    }
    if (!valid_label(label,macro_table)) {
        prer(lineNum,"Label name is invalid");
        return 0;
    }
    if (!valid_instruction(instruction)) {
        prer(lineNum,"Instruction name is invalid");
        return 0;
    }
    if (strcmp(".extern", instruction)==0) label = remainder;
    if (label) {
        /*Check whether the label was defined earlier*/
        if (!find_label(label_table,label)) add_label(label_table, label,instruction, IC, DC);
        else {
            prer(lineNum, "Label can only be defined once");
            return 0;
        }
    }


    /*
    printf("%s  %s  %s good line\n",label, instruction, remainder);
    */

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
void del_ending_spaces(char *str) {
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
    del_ending_spaces(remainder);

    return remainder;
}


LabelTable* create_label_table() {
    LabelTable *table = (LabelTable *)malloc(sizeof(LabelTable));
    /*Creating an empty table for the lables, with space for (start size) labels and it can be increased*/
    if (table == NULL) {
        fprintf(stderr, "Memory allocation failure\n");
        exit(1);
    }
    table->label_list = (LabelNode *)malloc(START_SIZE * sizeof(LabelNode));
    if (table->label_list == NULL) {
        fprintf(stderr, "Memory allocation failure\n");
        free(table);
        exit(1);
    }
    table->count = 0;
    table->space = START_SIZE;
    return table;
}


void add_label(LabelTable *table, const char *name, char *instruction, int address, int daddress) {
    int type;
    type = (strcmp(instruction, ".data")==0 || strcmp(instruction, ".string")==0) ? DATA : STD;
    if (strcmp(instruction, ".extern")==0) type = EXTERN;
    if (strcmp(instruction, ".entry")==0) {
        /*This will be processed in the second pass*/
        return;
    }

    /*We want to add label to the table but it's full, so we re allocating memory by increasing the space*/
    if (table->count >= table->space) {
        table->space += START_SIZE;
        table->label_list = (LabelNode *)realloc(table->label_list, table->space * sizeof(LabelNode));
        if (table->label_list == NULL) {
            fprintf(stderr, "Memory allocation failure\n");
            exit(1);
        }
    }

    /*Adding the new label in the last place (the first one that is empty), by editing the attributes of the structs*/
    strncpy(table->label_list[table->count].name, name, LABEL_SIZE);
    table->label_list[table->count].name[LABEL_SIZE] = '\0';
    /*Writing the adress as DC if it's of type data, otherwise it's IC*/
    if (type==DATA) {
        table->label_list[table->count].address = daddress;
    }
    else if (type==STD) table->label_list[table->count].address = address;
    else table->label_list[table->count].address = 0;
    table->label_list[table->count].type = type;
    table->count++;
}

int find_label(const LabelTable *table, const char *name) {
    int i;
    /*Seacrhing for the label with the given name in the label table*/
    for (i = 0; i < table->count; i++) {
        /*
        printf("%s\n", table->label_list[i].name);
        */
        if (strcmp(table->label_list[i].name, name) == 0) {
            return table->label_list[i].type;
        }
    }
    return 0;
}

int get_address(LabelTable *table, char *name) {
    int i;
    /*Seacrhing for the label with the given name in the label table*/
    for (i = 0; i < table->count; i++) {
        /*
        printf("%s\n", table->label_list[i].name);
        */
        if (strcmp(table->label_list[i].name, name) == 0) {
            return table->label_list[i].address;
        }
    }
    return 0;
}


void free_label_table(LabelTable *label_table) {
    /*If the table is in the memory, we free it*/
    if (label_table) {
        if (label_table->label_list) {
            free(label_table->label_list);
            label_table->label_list = NULL;
        }
        label_table = NULL;
    }
}



