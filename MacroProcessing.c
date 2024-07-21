/*
This file handles the pre-assembly stage by identifying and expanding macros
in the assembly code. It reads the input assembly file line by line, detects
macro definitions, and stores the macro names and their corresponding lines
of code. It ensures that macro names are valid and do not conflict with
operation or instruction names. The code also checks for syntax errors in
macro definitions and reports the errors. Finally, it generates an
expanded assembly file with all macros replaced by their definitions, ensuring
the assembly code is ready for the next stage of the assembly process.
 */


#include "MacroProcessing.h"



