#include "passTwo.h"
#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AssemblyConstants.h"
#include "helper.h"
#include "MacroProcessing.h"
#include "structs.h"

void pass_two(char *filename, LabelTable *label_table, int ErrorFlag) {
    char line[2345], *remainder, *label = NULL, *instruction = NULL, *op1, *op2,type1,type2;
    int cnt,lineNum = 0, op_count,DC = 0,  IC = ADDRESS_START, add,code,i;
    char enc[FNAME_SZ],ent[FNAME_SZ], ext[FNAME_SZ];
    strcpy(enc,filename);
    strcpy(ent,filename);
    strcpy(ext,filename);
    strcpy(enc+strlen(enc)-ENDING,".ob");
    strcpy(ent+strlen(ent)-ENDING,".ent");
    strcpy(ext+strlen(ext)-ENDING,".ext");

    FILE *file, *fenc, *fent, *fext;

    /*Creating the files that contain the resulting data, in "w" mode as the results are written there*/
    file = fopen(filename, "r");
    fenc = fopen(enc, "w");
    fent = fopen(ent, "w");
    fext = fopen(ext, "w");


    /*Validate that the files were creates without issues*/
    if (file == NULL || fenc == NULL || fent == NULL || fext == NULL) {
        printf("Unable to create the files");
        return;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;
        /*
        Skip the line if it's a comment or an empty line
        */
        if (line[0]==';' || line[0]=='\n') continue;
        remainder = get_line_remainder(line, &label, &instruction);
        /*
        printf("%s %s %s \n",label,instruction,remainder);
        */
        if (strcmp(instruction, ".entry")==0) {
            if(!sec_pass_valid_line(instruction, remainder, lineNum, label_table, op1, op2)) {
                ErrorFlag = 0;
                continue;
            }
        }
        if (strcmp(instruction, ".extern")==0) continue;
        /*data, string and extern and entry were processed already*/
        if (instruction[0]!='.'){
            /*Validating and processing the operations*/
            op_count = parse_operands(remainder,&op1,&op2,lineNum);
            type1 = get_operand_type(op1, lineNum);
            type2 = get_operand_type(op2, lineNum);

            if (op_count==ERR || !valid_oper_oper(type1,type2,instruction,lineNum,op_count)) {
                ErrorFlag = 0;
                continue;
            }
            /*Checks that */
            if(!sec_pass_valid_line(instruction, remainder, lineNum, label_table, op1, op2)) {
                ErrorFlag = 0;
                continue;
            }
            code = 0;
            code |= (get_opcode(instruction)<<OPCODE_BIT);
            printf("%s %s %s \n",instruction,op1,op2);
            for (i = 14; i >= 0; i--) {
                printf("%d", (code >> i) & 1);
            }
            printf("\n");
            /*encode()*/
        }
    }


    fclose(file);
    fclose(fenc);
    fclose(fent);
    fclose(fext);
    }


int sec_pass_valid_line(char *instruction, char *remainder, int lineNum, LabelTable *label_table, char *op1, char *op2) {
    int type_op1, type_op2;
    if (strcmp(instruction, ".entry")==0) {
        if (!find_label(label_table, remainder)) {
            prer(lineNum, "entry parameter was not defined as label in this file");
            return 0;
        }

    }

    type_op1=get_operand_type(op1, lineNum);
    type_op2=get_operand_type(op2, lineNum);
    if (type_op1==DIR) {
        if (!find_label(label_table, op1)) {
            prer(lineNum, "Operand is adressed using direct adressing, but was not defined as label");
            return 0;
        }
    }
    else if (type_op2==DIR) {
        if (!find_label(label_table, op2)) {
            prer(lineNum, "Operand is adressed using direct adressing, but was not defined as label");
            return 0;
        }
    }
    return 1;

}
    /*
    */
    /*FILE *file;
    char line[2345], *remainder, *label = NULL, *instruction = NULL, *op1, *op2,type1,type2;


    int cnt,lineNum = 0, op_count,DC = 0,  IC = ADDRESS_START, ErrorFlag = 1, add;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;
        /*Skip the line if it's a comment or an empty line; we already deleted all the starting spaces#1#
        if (line[0]==';' || line[0]=='\n') continue;
        /* Process the line #1#

        if (!validate_line(line,label,instruction,remainder,lineNum,macro_table,label_table,IC,DC)) {
            ErrorFlag = 0;
            continue;
        }

        if (instruction && instruction[0]=='.') {
            add = count_special_instruction(instruction, remainder, lineNum);
            if (add==ERR) {
                ErrorFlag = 0;
                continue;
            }
            DC+=add;
            continue;
        }
        op_count = parse_operands(remainder,&op1,&op2,lineNum);/*
        if (i!=ERR) printf("%d %s %s %d %d opes\n",i, op1, op2, get_operand_type(op1, lineNum), get_operand_type(op2, lineNum));
        else ErrorFlag = 0;#1#
        type1 = get_operand_type(op1, lineNum);
        type2 = get_operand_type(op2, lineNum);

        if (op_count==ERR || !valid_oper_oper(type1,type2,instruction,lineNum,op_count)) {
            ErrorFlag = 0;
            continue;
        }
        IC+=calc_IC(type1,type2);








        free(remainder);
        free(label);
        free(instruction);
    }

    fclose(file);
    return ErrorFlag;*/
int get_opcode(char *op) {
    int i;
    for (i=0; i<NUM_OF_OPERATIONS;i++) {
        if (strcmp(op,operations[i])==0) return i;
    }
    return ERR;
}