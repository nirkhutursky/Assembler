# Assembler

Assembler is a C90 implementation of a two-pass assembler for a custom instruction set. It expands macros, resolves labels and symbols, validates addressing modes, and emits machine-code output for assembly source files.

## Features

- Macro preprocessing before assembly.
- Two-pass compilation flow for symbol resolution and code generation.
- Support for labels and multiple addressing modes.
- Error reporting for invalid input and allocation failures.
- Manual memory management suitable for validation with tools such as Valgrind.

## Project Structure

- `main.c`: program entry point and top-level assembly flow.
- `MacroProcessing.c`: macro parsing and expansion.
- `passOne.c`: first-pass symbol collection and validation.
- `passTwo.c`: second-pass translation and output generation.
- `helper.c`: shared utility helpers.
- `AssemblyConstants.c`: instruction-set and assembler constants.
- `structs.h`: shared data structures.
- `test1.as`: sample assembly input.

## Requirements

- CMake 3.28 or newer.
- A C compiler with C90 support.

## Build

```bash
git clone https://github.com/NirKhutorsky/Assembler.git
cd Assembler
cmake -S . -B build
cmake --build build
```

## Usage

The program expects one or more input names without the `.as` suffix.

```bash
./build/Assembler test1
```

Given `test1`, the assembler reads `test1.as`, expands macros into `test1.am`, and then runs the two assembly passes on the expanded file.

## Notes

- The repository currently includes generated or local-development artifacts such as `ass.exe`, `cmake-build-debug`, and `.idea`.
- The documented build path above uses the checked-in `CMakeLists.txt`, which matches the current repository contents.
