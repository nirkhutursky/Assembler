#ifndef STRUCTS_H
#define STRUCTS_H


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



typedef struct Label {
    char *name;
    int address;
    int type;
    struct Label *next;
} Label;

typedef struct LabelTable {
    Label *head;
    Label *tail;
} LabelTable;

#endif /* STRUCTS_H */
