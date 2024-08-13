
#include <stdio.h>

/*COMPILE
  gcc -o ass MacroProcessing.c helper.c passOne.c passTwo.c main.c -std=c90 -Wall -Wextra -pedantic
  ./ass.exe testMacro test1

  To Do:
  entry labels - process - in the second pass check whether the entry label in the table
  in the second pass -check that operand labels are correct (in DIR addressing) - also, if they are extern, push into ext.ob

 */
#include <stdlib.h>
#include <string.h>

#include "MacroProcessing.h"
#include "helper.h"
#include "passOne.h"
#include "passTwo.h"
/*This project uses ansiC90*/



int main(int argc, char *argv[]) {
    int i,j;

    MacroTable *mt = create_macro_table();
    LabelTable *lt = create_label_table();
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_files>\n", argv[0]);
        return 1;
    }



    for (i = 1; i < argc; i++) {
        char output_filename[FNAME_SZ];
        strcpy(output_filename, argv[i]);
        strcat(argv[i], ".as");
        strcat(output_filename, ".am");
        if (parse_macros(argv[i], output_filename, mt)) {
            printf("Processing %s -> %s\n", argv[i], output_filename);
        } else {
            fprintf(stderr, "Error processing file: %s\n", argv[i]);
            remove(output_filename);
        }
            pass_two(output_filename,lt,pass_one(output_filename, mt, lt));

        for (j = 0; j < lt->count; j++) {
            printf("Label: %s, Address: %d Type: %d \n", lt->label_list[j].name, lt->label_list[j].address, lt->label_list[j].type);
        }
        free_macro_table(mt);
        mt = create_macro_table();
        free_label_table(lt);
        lt = create_label_table();

    }

    free_macro_table(mt);
    free_label_table(lt);


    return 0;
}