
#include "helper.h"
#include <string.h>


/* This functions adds .as ending to a given filer */
void get_out_name(const char *input_filename, char *output_filename) {
    strcpy(output_filename, input_filename);
    strcat(output_filename, ".as");
}
