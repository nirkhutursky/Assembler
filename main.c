
#include <stdio.h>

/*COMPILE
  gcc -o ass AssemblyConstants.c MacroProcessing.c helper.c passOne.c passTwo.c main.c -std=c90 -Wall -Wextra -pedantic
  ./ass.exe testMacro test1

  To Do:
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

    /*Initializing the label table and the macro table*/
    mt = create_macro_table();
    lt = create_label_table();




    for (i = 1; i < argc; i++) {
        char input_filename[FNAME_SZ];
        char output_filename[FNAME_SZ];

        strcpy(input_filename, argv[i]);
        strcat(input_filename, ".as");

        strcpy(output_filename, argv[i]);
        strcat(output_filename, ".am");

        printf("%s\n", input_filename);

        if (parse_macros(input_filename, output_filename, mt)) {
            printf("Parsing macros in %s to the file %s\n", input_filename, output_filename);
        } else {
            fprintf(stderr, "Error in parsing the file: %s\n", input_filename);
            remove(output_filename);
            continue;
        }
        /*The returned value tells whether there was an error in the first pass*/
        ErrorFlag = pass_one(output_filename, mt, lt, &DC);
        printf("Translating the file %s to machine code\n", argv[i]);
        pass_two(output_filename,lt,ErrorFlag,DC);
        /*Freeing the memomry allocated to the macro and label tables*/
        free_macro_table(mt);
        mt = create_macro_table();
        free_label_table(lt);
        lt = create_label_table();

    }




    return 0;
}