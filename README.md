# C-assembler 


Imaginary assembler implementation in c language, its purpose is to translate a custom assembly language into an intermediate format using a base-8 (octal) number system, with each translated word represented by 5 digits.

When the assembler is run on a .as source file, it processes the content, expands macros (if any), identifies labels and instructions, and produces several output files depending on the structure and correctness of the input file

If the assembly source file is valid, the following files may be generated:

.ob file – The main output file. It contains the translated code in octal base, with each word represented using 5-digit octal numbers.

.am file – This file is created if macros are defined in the original source. It contains the expanded code, with all macros replaced by their full content. This is the preprocessed version ready for translation.

.ent file – Created only if there are .entry declarations in the code. It lists all entry labels along with their addresses, which are represented in decimal.

.ext file – Similar to the .ent file, but for external labels defined using .extern. It includes external label names and their decimal addresses. The original source file remains unchanged.

If the assembly file contains issues, there are two possible outcomes:

Warning – The translation continues, but the user is notified about non-critical issues that may need attention.

Error – A critical error is found, and the translation process stops. The user must fix the error before continuing.

This project focuses solely on the assembler’s translation stage. It does not include the linker stage
