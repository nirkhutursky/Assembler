

#ifndef STRUCTS_H
#define STRUCTS_H

/* Data Structures that are used in all stages of the processing*/

/* A struct that represents a line in the assembly code */
typedef struct Line {
    char *value;
    int number;
    struct Line *next; /* Pointer to the next line in the linked list */
} Line;

#endif /*STRUCTS_H*/
