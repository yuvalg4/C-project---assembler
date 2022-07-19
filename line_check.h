#ifndef LINE_CHECK_H
#define LINE_CHECK_H
#include "globals.h"

#define LABEL_EXTENSION 10
#define FIRST_REGISTER_ALLOWED 10
#define LINE_TOO_LONG_ERROR 1
#define MISSING_OPERAND_ERROR 2
#define ILLEGAL_DATA_ERROR 3
#define INVALID_OPERAND_TYPE_ERROR 4
#define MULTIPLE_COMMAS_ERROR 5
#define MISSING_COMMA_ERROR 6
#define TEXT_AFTER_END_ERROR 7
#define ILLEGAL_OPERAND_ERROR 8
#define ILLEGAL_COMMA_ERROR 9
#define UNDEFINE_COMMAND_NAME_ERROR 10
#define ILLEGAL_LABEL_NAME_ERROR 11
#define LABEL_EXIST_ERROR 12
#define ILLEGAL_STRING_ERROR 13
#define LABEL_WITHOUT_EXTERN_ERROR 14

/*
param: line[] - string of assembly command
param: *assembly - pointer to assembly FILE object (.am file)
param: *label_array - array of label structs
param: arr_last - length of array
param: line_num - number of line in the file.
return: 1 if error is found. 0 otherwise.

the function interates over each line and looks for errors. error message will printed if found.
*/
int is_error(char line[], FILE* assembly, label* label_array, int arr_last,int line_num);

#endif