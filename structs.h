#ifndef STRUCTS_H
#define STRUCTS_H
#include "AssemblyConstants.h"


typedef struct Line {
    char *value;
    int number;
    struct Line *next;
} Line;

typedef struct Macro {
    char *name;
    Line *lines;  /* Pointer to the head of the linked list of lines */
    struct Macro *next;
} Macro;

typedef struct MacroNode {
    char *name;
    Macro *macro;
    struct MacroNode *next;
} MacroNode;

typedef struct MacroTable {
    MacroNode **table;  /* Pointer to an array of pointers to MacroNode */
    int size;
} MacroTable;



typedef struct {
    char name[LABEL_SIZE + 1];
    int address;
} LabelNode;

typedef struct {
    LabelNode *label_list;
    int count;
    int space;
} LabelTable;

#endif /* STRUCTS_H */
