#include "passTwo.h"
#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AssemblyConstants.h"
#include "helper.h"
#include "structs.h"

void pass_two(char *filename, LabelTable *label_table, int ErrorFlag, int DC) {
    char line[BIG_LEN], *remainder, *label = NULL, *instruction = NULL, *op1, *op2,type1,type2, *strArr;
    int *dataArr,cnt,len,lineNum = 0, op_count,extFlag=0,entFlag = 0,  IC = ADDRESS_START,i,machineCode[CODESIZE];
    size_t j;
    signed int opbincode,opercode1, opercode2;
    char enc[FNAME_SZ],ent[FNAME_SZ], ext[FNAME_SZ];
    FILE *file, *fenc, *fent, *fext;
    strcpy(enc,filename);
    strcpy(ent,filename);
    strcpy(ext,filename);
    strcpy(enc+strlen(enc)-ENDING,".ob");
    strcpy(ent+strlen(ent)-ENDING,".ent");
    strcpy(ext+strlen(ext)-ENDING,".ext");



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
        if (line[0]==';' || line[0]=='\n' || strlen(line)>LINE_SIZE) continue;
        remainder = get_line_remainder(line, &label, &instruction);
        /*
        printf("%s %s %s \n",label,instruction,remainder);
        */
        if (strcmp(instruction, ".entry")==0) {
            /*Checking entry labels, if valid we add them to the entry file, otherwise mark as error*/
            if(!sec_pass_valid_line(instruction, remainder, lineNum, label_table, op1, op2)) {
                ErrorFlag = 0;
            }
            else {
                /*Adding entry label to the file with its address*/
                fprintf(fent,"%s %d\n",remainder, get_address(label_table,remainder));
                entFlag = 1;
            }
            continue;
        }
        if (strcmp(instruction, ".extern")==0) continue;
        /*data, string and extern and entry were processed already*/
        if (instruction[0]!='.'){
            /*Validating and processing the operations*/
            op_count = parse_operands(remainder,&op1,&op2);
            if (op_count==ERR) continue;
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
            /*Encoding the operation with the addressing types*/
            opbincode = encbin(instruction, type1, type2);
            if (IC>=CODESIZE) {
                /*Address is out of machine code array bounds*/
                prer(lineNum, "There is no more space for writing machine code");
                ErrorFlag = 0;
                break;
            }
            /*If both source and dest exist*/
            if (type1!=NONE) {
                /*First operand is source, second one is destination*/
                /*Adding extern label to the file with its address*/
                if (find_label(label_table, op1)==EXTERN) {
                    fprintf(fext,"%s %04d\n", op1, IC+O_OP);
                    extFlag = 1;
                }
                if (find_label(label_table, op2)==EXTERN) {
                    fprintf(fext,"%s %04d\n", op2, IC+T_OP);
                    extFlag = 1;
                }
                /*Encodings of both operands*/
                opercode1 = encbinoper(op1, type1, 0, label_table);
                opercode2 = encbinoper(op2, type2, 1, label_table);
                /*If both operands are direct/undirect register addressing*/
                if (type1*type2>=THRESH) {
                    opercode1 |= opercode2;
                    machineCode[IC] = opbincode;
                    IC++;
                    machineCode[IC] = opercode1;
                    IC++;
                }
                else {
                    machineCode[IC] = opbincode;
                    IC++;
                    machineCode[IC] = opercode1;
                    IC++;
                    machineCode[IC] = opercode2;
                    IC++;
                }
            }
            else if (type2!=NONE) {
                /*Adding extern label to the file with its address*/
                if (find_label(label_table, op2)==EXTERN) {
                    fprintf(fext,"%s %04d\n", op2, IC+O_OP);
                    extFlag = 1;
                }
                /*if there is only one operand */
                opercode1 = encbinoper(op2, type2, 1, label_table);
                machineCode[IC] = opbincode;
                IC++;
                machineCode[IC] = opercode1;
                IC++;
            }

            else {
                /*Zero operands*/
                machineCode[IC] = opbincode;
                IC++;

            }

        }
        else {
            /*Adding the array values into the code, in DC positions*/
            if (strcmp(instruction, ".data")==0) {
                dataArr = parse_numbers(remainder, &cnt);
                for (i=0; i<cnt; i++) {
                    machineCode[DC+i] = dataArr[i];
                }
                DC+=cnt;
                free(dataArr);

            }
            /*Adding the ascii value of the chars in the string into the code, in DC positions*/
            if (strcmp(instruction, ".string")==0) {
                strArr = parse_word(remainder);
                len = strlen(strArr);
                for (j=0; j<strlen(strArr)+1; j++) {
                    machineCode[DC+j] = (int)strArr[j];
                }
                DC+=len;
                machineCode[DC] = '\0';
                DC++;
                free(strArr);
            }
        }

        free(remainder);
        free(label);
        free(instruction);
    }
    if (!ErrorFlag) {
        /*There was an error either in the first or the second pass*/
        fclose(fenc);
        fclose(fext);
        fclose(fent);
        remove(enc);
        remove(ent);
        remove(ext);
        printf("Assembler files were not created because of errors\n");
    }
    else {
        printf("Files were created successfully\n");
        /*Printing length of IC and length of DC*/

        fprintf(fenc,"  %d %d\n", IC-ADDRESS_START,DC-IC);
        /*Printing into the file the coded words and their addresses*/
        for (i=ADDRESS_START; i<DC; i++) {
            /*Using this mask and specifier ensures that we print exactly 5 digits (with zeroes if needed)*/
            fprintf(fenc,"%04d %05o\n",i, machineCode[i] & FIVE_OCT_DIG);
        }
        /*Deleting the ext\ent files if there were no extern/entry labels*/
        if (!extFlag) {
            fclose(fext);
            remove(ext);
        }
        if (!entFlag) {
            fclose(fent);
            remove(ent);
        }
    }
    fclose(file);
    fclose(fenc);
    fclose(fent);
    fclose(fext);
    }


int sec_pass_valid_line(char *instruction, char *remainder, int lineNum, LabelTable *label_table, char *op1, char *op2) {
    int type_op1, type_op2, labelType;
    if (strcmp(instruction, ".entry")==0) {
        /*Label that was declared with entry can't be defined in the current file, nor can it be extern*/
        labelType = find_label(label_table, remainder);
        if (!labelType) {
            prer(lineNum, "Entry parameter was not defined as label in this file");
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
    /*Checking that all operands that are used as operands(direct addressing), are defined as labels somewhere in the file*/
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
/*This function calculates the opcode of a given operation*/
int get_opcode(char *op) {
    int i;
    for (i=0; i<NUM_OF_OPERATIONS;i++) {
        /*The position of the operation in the table is its opcode*/
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
        /*Shifting the bits according to src\dst and the resulting number*/
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
        /*Getting the address and the ARE bit, and shifting the bits accordingly*/
        num = get_address(label_table, op);
        are = find_label(label_table, op);
        if (are==EXTERN) num = (num<<ADR_SP)|(1<<E);
        else num = (num<<ADR_SP)|(1<<R);
        return num;
    }
    return ERR;
}
