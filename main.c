
#include <stdio.h>

/*COMPILE
  gcc -o ass AssemblyConstants.c MacroProcessing.c helper.c passOne.c passTwo.c main.c -std=c90 -Wall -Wextra -pedantic
  ./ass.exe testMacro test1

  To Do:
  fill result files if there were no errors (and if entry and extern are present)
  clear warnings, comentate, change some variables (antigpt)
  move to ubuntu (run dos2unix with gpt),create make file, make sure it works there
  fix if needed memory leaks with valgrind with gpt

 */
#include <string.h>
#include "MacroProcessing.h"
#include "helper.h"
#include "passOne.h"
#include "passTwo.h"



int main(int argc, char *argv[]) {
    int i,DC,ErrorFlag;
    MacroTable *mt;
    LabelTable *lt;


    mt = create_macro_table();
    lt = create_label_table();
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
            continue;
        }
            ErrorFlag = pass_one(output_filename, mt, lt, &DC);
            pass_two(output_filename,lt,ErrorFlag,DC);
            /*
            printf("%d\n", DC);
            */


        /*for (j = 0; j < lt->count; j++) {
            printf("Label: %s, Address: %d Type: %d \n", lt->label_list[j].name, lt->label_list[j].address, lt->label_list[j].type);
        }*/
        free_macro_table(mt);
        mt = create_macro_table();
        free_label_table(lt);
        lt = create_label_table();

    }

    free_macro_table(mt);
    free_label_table(lt);


    return 0;
}