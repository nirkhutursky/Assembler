Assembler
Overview
This project is an assembler written in C for a custom computer architecture. It translates assembly code into machine code, handling macros, labels, and various addressing modes. The assembler processes input files, parses macros, and generates binary code, following a two-pass method.

Features
Macro Processing: Supports macro definitions and usage within the assembly code.
Two-Pass Assembly:
Pass One: Builds a symbol table and handles labels.
Pass Two: Translates instructions and generates machine code.
Error Handling: Detects and reports syntax errors and memory allocation issues.
Memory Management: Properly allocates and frees memory to avoid leaks, as verified using Valgrind.
Support for Custom Architecture: Implements a specific instruction set and addressing modes, including immediate, direct, indirect register, and direct register addressing.
Installation
To build and run the assembler, you need a GCC compiler and Make utility installed on your system.

Clone the repository
bash
Copy code
git clone https://github.com/nirkhutursky/Assembler.git
cd Assembler
Build the project
bash
Copy code
make
Run the assembler
bash
Copy code
./assembler <input_file>
Usage
Provide the assembler with an assembly file as input. The assembler will generate corresponding machine code output files.

Example:

bash
Copy code
./assembler test1
File Structure
main.c: Main function to handle the input and manage the assembly process.
MacroProcessing.c: Handles macro definitions and usage.
passOne.c: First pass of the assembler, building the symbol table and processing labels.
passTwo.c: Second pass of the assembler, translating instructions into machine code.
helper.c: Utility functions used across the assembler.
AssemblyConstants.c: Contains constants used in the assembly process.
Testing and Debugging
Valgrind: Memory management and leak detection were verified using Valgrind. The assembler has been tested to ensure no memory leaks.
Contributing
If you want to contribute to this project, feel free to submit issues or pull requests.

License
This project is licensed under the MIT License.
