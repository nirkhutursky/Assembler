#include <stdio.h>
#include <ctype.h>
#include <string.h>


/*COMPILE
  gcc -o ass MacroProcessing.c main.c -std=c90 -Wall -Wextra -pedantic
  ./ass.exe testMacro test1
 */
#include "MacroProcessing.h"

/*This project uses ansiC90*/


char* generate_output_filename(const char *input_filename, char *output_filename) {
    strcpy(output_filename, input_filename);
    strcat(output_filename, ".as");
    return output_filename;
}

int main(int argc, char *argv[]) {
    int i;
    char* output;
    if (argc < 2) {
        return 1;
    }

    for (i = 1; i < argc; i++) {
        char output_filename[256];
        output = generate_output_filename(argv[i], output_filename);
        if (parse_macros(argv[i], output_filename)){
            printf("Processing %s -> %s\n", argv[i], output_filename);

        }
        else {
            remove(output_filename);
        }
    }

    return 0;
}