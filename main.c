#include <stdio.h>

/*COMPILE
  gcc -o ass MacroProcessing.c helper.c main.c -std=c90 -Wall -Wextra -pedantic
  ./ass.exe testMacro test1
 */
#include "MacroProcessing.h"
#include "helper.h"
/*This project uses ansiC90*/



int main(int argc, char *argv[]) {
    int i;
    if (argc < 2) {
        return 1;
    }

    for (i = 1; i < argc; i++) {
        char output_filename[256];
        get_out_name(argv[i], output_filename);
        if (parse_macros(argv[i], output_filename)){
            printf("Processing %s -> %s\n", argv[i], output_filename);

        }
        else {
            remove(output_filename);
        }
    }

    return 0;
}