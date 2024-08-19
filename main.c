
#include <stdio.h>


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





    for (i = 1; i < argc; i++) {
        mt = create_macro_table();
        lt = create_label_table();
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
            free_macro_table(mt);
            free_label_table(lt);
            continue;
        }
        /*The returned value tells whether there was an error in the first pass*/
        ErrorFlag = pass_one(output_filename, mt, lt, &DC);
        printf("Translating the file %s to machine code\n", argv[i]);
        pass_two(output_filename,lt,ErrorFlag,DC);
        /*Freeing the memomry allocated to the macro and label tables*/
        free_macro_table(mt);
        free_label_table(lt);

    }




    return 0;
}