# C project - assembler

Task Summery: Create an assembler for fictional assembly language defined by openu course.
Implementation Overview and general approach:
Objectives (Goals):
Efficiency in memory (It was important to us allocate memory dynmically and make its size fit exact as required for each specific memory image output of the program.

Seperation of concern: we tried to split the program to indepent components that will be responsible for well defined tasks and that the sharing of data between files will be through some kind of encapsolution method such as state getters/setters and scoping global variable to be static so that they will be scoped to the single file that they belongs to.

Absctration of data structures and variable use to fit our needs by definning different typedef structs types enums and constants.

Our implimentation Overview in short:
Stages:
Pre Assembler - parsing the original .as file and replacing macros if existing.

First Pass - to first run in which we will saves all the labels, data in their corresponding array while updating DC and IC, and will verify that there are no errors in code.

Second Pass - our assembler writes the words of each line of code in binary format, we inserting the words to the memory image in the correct position depending on the type of word (data/instruction) and we add each external operand appearence to the linked list of all the positions that external operands appeared. if we encouter label operand that is not within the symbol table and is not external we yield error message and continue checking the rest of the code in order to discover all errors of this type if this case happens we will finish second run but will not export any files.

Exporting files, after second run Exporting all the required files (.ob, .ext .ent) and moves to the next file or end program.
