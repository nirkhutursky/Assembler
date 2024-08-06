

#ifndef HELPER_H
#define HELPER_H


/*This functions adds .as ending to a given file
 * Gets the name of the input and output files
 * Changes the outputfile by pointer
 */
void get_out_name( char *input_filename, char *output_filename);

int is_label(const char *line, char **label) ;

int is_instruction(const char *line);

void prer(int lineNum, char* error_type);

int* parse_numbers(const char *line, int *num_count);

char* parse_word(const char *line);

int count_special_instruction(char *instruction, char *remainder, int lineNum);
#endif /*HELPER_H*/
