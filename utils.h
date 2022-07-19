#ifndef UTILS_H
#define UTILS_H
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

#define COMMA ','
#define DIGITS_BASE_CHANGE 4
#define BASE_CHANGE_SIZE 14
#define BINARY_BASE 2
#define ASCII_OFFSET 10
#define A_ASCII 65
#define TRUE 1
#define FALSE 0
#define OBJECT_FILE ".ob"
#define ENTRY_FILE ".ent"
#define WRITE "w"
#define STRING ".string"
#define DATA ".data"
#define EXTERN ".extern"
#define ENTRY ".entry"
#define ZERO_CHAR '0'
#define A_CHAR 'A'
#define NOT_IN_ARRAY -1
#define FIRST_REGISTER_ALLOWED 10
#define END_FILE_EXPRESSION_LENGTH 4
#define NO_PARAMETERS 0
#define ONE_PARAMETER 1
#define TWO_PARAMETERS 2
#define MSG_LENGTH 60

/*
param: i - given index
param: row[] - command string
returns: 1 if end of line. 0 otherwise.
*/
int end_of_line (int i, char row[]);

/*
param: i - given index
param: row[] - command string
returns: index after whitespace
*/
int skip_white_char (int i, char row[]);

/*
param: operand[] - string of operand
returns: 1 if given string is a legal label name. 0 otherwise.
*/
int llegal_label_name(char operand[]);

/*
param: operand[] - string of operand
returns: 1 if operand is register. 0 otherwise.
*/
int is_register(char operand[]);

/*
param: operand[] - string of operand
returns: 1 if operand is immediate. 0 otherwise.
*/
int is_immediate(char operand[]);

/*
param: command[] - string of command
returns: 1 if command is of data type. 0 otherwise.
*/
int is_data(char command[]);

/*
param: command[] - string of command
returns: 1 if command is of string type. 0 otherwise.
*/
int is_string(char command[]);

/*
param: command[] - string of command
returns: 1 if command is of entry type. 0 otherwise.
*/
int is_entry(char command[]);

/*
param: command[] - string of command
returns: 1 if command is of extern type. 0 otherwise.
*/
int is_extern(char command[]);

/*
param: operand[] - string of operand
returns: 1 if command is of label type. 0 otherwise.
*/
int is_label(char operand[]);

/*
param: word[] - string
removes the last char of a given string.
*/
void remove_last_char(char *word);

/*
param: row[] - string of command
param: *split_row[] - will contain the subarrays after the split.
param: delimt[] - delimiter of strtok
returns: length of split_row
*/
int split_command(char row[],char *split_row[], char delimt[]);

/*
param: bin_code[] - binary code array
param: start - start index
param: finish - end index
param *after_base_change - string that will accept the hex number.
param: array_index - index to add the hex number.
this function will change the base from binary to hex of a given range.
*/
void bin_to_hex(int bin_code[],int start,int finish,char *after_base_change,int array_index);

/*
param: bin_code[] - binary code array
returns: binary code representation in "special base" given in the assignment.
*/
char *change_base(int bin_code[]);

/*
param: input_num - decimal number that will be converted to binary.
param: arr[] - array that will be assigned the binary number.
this function converts decimal to binary. will only work with empty array.
*/
void decimal_to_binary(int input_num, int bin_array[]);

/*
param: num - decimal number that will be converted to binary.
param: bin_array[] - array that will be assigned the binary number.
param: start - start index
param: end - end index
converts positive decimal to binary and inserts in a given range.
*/
void decimal_to_binary_pos(int num,int bin_array[],int start, int end);

/*
param: label_name[] - string of a label name.
param: arr[] - array that contains all the label structs.
param: end_index - length of the label array.
returns: 1 if label_name is found in the label array. 0 otherwise.
*/
int in_array_label(char label_name[], label arr[], int end_index);

/*
param: operand[] - operand
returns: 1 if operand is of index addressing type. 0 otherwise.
*/
int is_index_addressing_mode(char operand[]);

/*
param: *row_index - index of row
param: row[] - string of command
param: next_word[] - will hold the next word in command
the function will read the next word from the command row
*/
void read_word (int *row_index, char row[], char next_word[]);

/*
param: file_name[] - name of the file.
param: file_type[] - type of the file (.ob,.ent,...)
returns: pointer to the new file.
*/
FILE *open_and_name_new_file(char file_name[], char file_type[]);

/*
param: command[] - string of command
returns: 1 if no parameters in command. 0 otherwise
*/
int no_parameters_command(char command[]);

/*
param: command[] - string of command
returns: 1 if one parameter in command. 0 otherwise
*/
int one_parameters_command(char command[]);

/*
param: command[] - string of command
returns: 1 if two parameters in command. 0 otherwise
*/
int two_parameters_command(char command[]);

/*
param: msg_num - int of message number
param: row_num - number of row in file
if error is found prints an error message.
*/
void print_msg(int msg_num, int row_num);

#endif
