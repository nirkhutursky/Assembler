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

void pass_two(char *filename, LabelTable *label_table, int ErrorFlag, int DC) {
    char line[2345], *remainder, *label = NULL, *instruction = NULL, *op1, *op2,type1,type2, *strArr;
    int *dataArr,cnt,len,lineNum = 0, op_count,  IC = ADDRESS_START, i,j,machineCode[4096];
    signed int opbincode,opercode1, opercode2;
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
    printf("\n%d\n",DC);
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
            }
            else {
                printf("ENTRY: %s %d\n",remainder, get_address(label_table,remainder));
            }
            continue;
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
            /*Checks that operands that are used aas labels were defined in the file*/
            if(!sec_pass_valid_line(instruction, remainder, lineNum, label_table, op1, op2)) {
                ErrorFlag = 0;
                continue;
            }
            /*If there is only one operand, it's the destination operand*/
            if (type2==NONE) {
                op2 = op1;
                type2 = type1;
                type1 = NONE;
                op1 = NULL;
            }
            opercode1 = 0;
            opercode2 = 0;

            opbincode = encbin(instruction, type1, type2);
            /*If both source and dest exist*/
            if (type1!=NONE) {
                /*First operand is source, second one is destination*/
                if (find_label(label_table, op1)==EXTERN) {
                    printf("EXTERN: %s %d\n", op1, IC+O_OP);
                }
                if (find_label(label_table, op2)==EXTERN) {
                    printf("EXTERN: %s %d\n", op2, IC+T_OP);
                }
                opercode1 = encbinoper(op1, type1, 0, label_table);
                opercode2 = encbinoper(op2, type2, 1, label_table);
                /*If both operands are direct/undirect register addressing*/
                if (type1*type2>=THRESH) {
                    opercode1 |= opercode2;
                    machineCode[IC] = opbincode;
                    IC++;
                    machineCode[IC] = opercode1;
                    IC++;

                    /*for (i = 14; i >= 0; i--) {
                        printf("%d", (opbincode >> i) & 1);
                    }
                    printf("\n");
                    for (i = 14; i >= 0; i--) {
                        printf("%d", (opercode1 >> i) & 1);
                    }
                    printf("\n");*/
                }
                else {
                    machineCode[IC] = opbincode;
                    IC++;
                    machineCode[IC] = opercode1;
                    IC++;
                    machineCode[IC] = opercode2;
                    IC++;
                    /*for (i = 14; i >= 0; i--) {
                        printf("%d", (opbincode >> i) & 1);
                    }
                    printf("\n");
                    for (i = 14; i >= 0; i--) {
                        printf("%d", (opercode1 >> i) & 1);
                    }
                    printf("\n");
                    for (i = 14; i >= 0; i--) {
                        printf("%d", (opercode2 >> i) & 1);
                    }
                    printf("\n");*/
                }



            }
            else if (type2!=NONE) {
                if (find_label(label_table, op2)==EXTERN) {
                    printf("EXTERN: %s %d\n", op2, IC+O_OP);
                }
                /*if there is only one operand */
                opercode1 = encbinoper(op2, type2, 1, label_table);
                machineCode[IC] = opbincode;
                IC++;
                machineCode[IC] = opercode1;
                IC++;
                /*for (i = 14; i >= 0; i--) {
                    printf("%d", (opbincode >> i) & 1);
                }
                printf("\n");
                for (i = 14; i >= 0; i--) {
                    printf("%d", (opercode1 >> i) & 1);
                }
                printf("\n");*/
            }

            else {
                machineCode[IC] = opbincode;
                IC++;
                /*for (i = 14; i >= 0; i--) {
                    printf("%d", (opbincode >> i) & 1);
                }*/
            }

            printf("%s %s %s %d %d %d\n",instruction,op1,op2, type1, type2, IC);
            /*encode()*/

        }
        else {
            if (strcmp(instruction, ".data")==0) {
                dataArr = parse_numbers(remainder, &cnt);
                for (i=0; i<cnt; i++) {
                    machineCode[DC+i] = dataArr[i];
                }
                DC+=cnt;

            }
            if (strcmp(instruction, ".string")==0) {
                strArr = parse_word(remainder);
                len = strlen(strArr);
                for (i=0; i<strlen(strArr)+1; i++) {
                    machineCode[DC+i] = (int)strArr[i];
                }
                DC+=len;
                machineCode[DC] = '\0';
                DC++;
            }
            printf("%s %s %d\n",instruction, remainder, DC);
        }

        free(remainder);
        free(label);
        free(instruction);
    }
    for (i=ADDRESS_START; i<DC; i++) {
        for (j = 14; j >= 0; j--) {
            printf("%d", (machineCode[i] >> j) & 1);
        }
        /*
        printf("%d\n",machineCode[i]);
        */
        printf(" %d\n",i);
    }

    fclose(file);
    fclose(fenc);
    fclose(fent);
    fclose(fext);
    }


int sec_pass_valid_line(char *instruction, char *remainder, int lineNum, LabelTable *label_table, char *op1, char *op2) {
    int type_op1, type_op2, labelType;
    if (strcmp(instruction, ".entry")==0) {
        labelType = find_label(label_table, remainder);
        if (!labelType) {
            prer(lineNum, "entry parameter was not defined as label in this file");
            return 0;
        }
        if (labelType==EXTERN) {
            prer(lineNum, "Label can not be defined both as entry and as extern in one file");
            return 0;
        }
        return 1;
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

int encbin(char *instruction, int type1, int type2) {
    int code;
    code = 0;
    /*First we set the bits of the opcode (11-14)*/
    code |= (get_opcode(instruction)<<OPCODE_BIT);
    if (type1!=NONE) {
        /*If source operand exists, turn on the bit in its position*/
        code |= (1<<(type1+SRC_BIT));
    }
    if (type2!=NONE) {
        /*If destination operand exists, turn on the bit in its position*/
        code |= (1<<(type2+DST_BIT));
    }
    /*Turn on the A bit for the first word*/
    code |= (1<<A);
    return code;
}

int encbinoper(char *op, int type, int where, LabelTable *label_table) {
    /*Signed int saves the nubmer is 2's complement*/
    signed int are, reg, num;
    num = 0;
    if (type==DIR_REG || type==UNDIR_REG) {
        /*We ignore the '*' if present, and the 'r'*/
        if (op[0]=='*') {
            op++;
        }
        op++;
        reg = op[0]-'0';
        if (where) num |= reg<<REG_DST_BIT;
        else num |= reg<<REG_SRC_BIT;
        num |= (1<<A);
        return num;

    }
    if (type==IMME) {
        num =  getNumber(op);
        /*Shifting 3 bits to left to give space for the adressing type bit*/
        num = (num<<ADR_SP)|(1<<A);
        return num;
    }
    if (type==DIR) {
        num = get_address(label_table, op);
        are = find_label(label_table, op);
        if (are==EXTERN) num = (num<<ADR_SP)|(1<<E);
        else num = (num<<ADR_SP)|(1<<R);
        return num;
    }
    return ERR;
}
