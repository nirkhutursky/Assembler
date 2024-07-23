#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "MacroProcessing.h"

/*This project uses ansiC90*/

int main(int argc, char *argv[]) {
    int i;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <output_file> <input_file1> [<input_file2> ... <input_fileN>]\n", argv[0]);
        return 1;
    }

    /* The first argument is the output file */
    const char *output_file = argv[1];

    /* Process each input file */
    for (i = 2; i < argc; i++) {
        printf("Processing %s...\n", argv[i]);
        parse_macros(argv[i], output_file);
    }

    return 0;
}
