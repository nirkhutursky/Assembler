#include "passOne.h"
#include <stdio.h>
#include <stdlib.h>

#include "helper.h"
#include "MacroProcessing.h"
#include "structs.h"







int print_labels_content(const char *filename) {
    FILE *file;
    char line[81];
    char *lab = NULL;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        /* Check if the line contains a label */
        if (is_instruction(line)) {
            printf("%s\n", line);
            free(lab); /* Free the allocated memory for the label */
            lab = NULL; /* Reset the pointer to NULL */
        }
    }

    fclose(file);
    return 1;
}


