#include "AssemblyConstants.h"

/*The definitions of the machine memory, including the registers, operations, and instructions*/
char *registers[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};
/* The operations in our custom assembly language  */
char *operations[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* The instructions in our custom assembly language  */
char *instructions[] = {
    ".data", ".string", ".entry", ".extern"
};
