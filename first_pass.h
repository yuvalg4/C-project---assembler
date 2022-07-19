#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include <stdio.h>
#include "globals.h"
#define STARTING_IC 100
#define STARTING_DC 0
#define DYNAMIC_ARRAY_INITIAL_SIZE 10
#define DIVIDE_BASE_OFFSET 16
#define STRING_DELIMITER "\""
#define NUMBER_OF_ELEMENTS_STRING_COMMAND 3
#define STRING_INDEX 1
#define EXTERNAL_BASE_OFFSET 0
#define BASE_OFFSET_DC_INCREMENT 2
#define BASE_NUM_OF_LINES 1
#define MAX_LINES_IN_FILE 8192
#define FILE_TOO_LONG_ERROR 17
#define BIGGEST_NUMBER 32767
#define SMALLEST_NUMBER -32768
#define NUMBER_NOT_IN_RANGE_ERROR 18

/*
param: *file_ptr - FILE pointer to assembly file after macros are inserted.
param: **label_array - pointer to array of label structs
param: *label_array_index - pointer to length of label array
param: **data_array - pointer to array of data structs
param *data_array_index - pointer to length of data array
param: *IC - pointer to IC variable
param: *DC - pointer to DC variable
return: returns 1 if no errors. 0 otherwise.

the function saves all the labels,data in their corresponding array while updating DC and IC.
*/
int first_pass(FILE *file_ptr,label **label_array,int *label_array_index, data **data_array,int *data_array_index,int *IC,int *DC);

#endif