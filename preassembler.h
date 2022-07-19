#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#define TRUE 1
#define FALSE 0
#define MACRO_EXTENTION 10
#define END_FILE_EXPRESSION_LENGTH 4
#define ADD_TO_LINE_LENGTH 2

/*
param: *assembly - FILE pointer to assembly file.
param: file_name[] - char array of file name

the function inserts all the macros into the file and creates a new file with the same name and a .am ending.
*/
void pre_assembler(FILE* assembly, char file_name[]);


#endif