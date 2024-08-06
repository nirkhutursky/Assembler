#include <stdio.h>

/*COMPILE
  gcc -o ass MacroProcessing.c helper.c passOne.c main.c -std=c90 -Wall -Wextra -pedantic
  ./ass.exe testMacro test1
 */
#include <stdlib.h>

#include "MacroProcessing.h"
#include "helper.h"
#include "passOne.h"
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
        char output_filename[256];
        get_out_name(argv[i], output_filename);
        if (parse_macros(argv[i], output_filename, mt)) {
            printf("Processing %s -> %s\n", argv[i], output_filename);
        } else {
            fprintf(stderr, "Error processing file: %s\n", argv[i]);
            remove(output_filename);
        }
        print_content(output_filename, mt, lt);
        for (j = 0; j < lt->count; j++) {
            printf("Label: %s, Address: %d\n", lt->label_list[j].name, lt->label_list[j].address);
        }
        lt->count = 0;
        free_macro_table(mt);
        mt = create_macro_table();

    }


    free_label_table(lt);


    return 0;
}