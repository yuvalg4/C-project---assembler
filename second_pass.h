#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#define SECOND_PASS_COMMAND_DELIMITER " \t\n,[]"
#define A 18
#define R 17
#define E 16
#define IMMEDIATE_ADDRESSING_MODE 0
#define DIRECT_ADDRESSING_MODE 1
#define INDEX_ADDRESSING_MODE 2
#define REGISTER_ADDRESSING_MODE 3 
#define START_IND_SOURCE_REGISTER 8
#define END_IND_SOURCE_REGISTER 11
#define START_IND_TARGET_REGISTER 2
#define END_IND_TARGET_REGISTER 5
#define START_IND_SOURCE_ADDRESSING_MODE 6
#define END_IND_SOURCE_ADDRESSING_MODE 7
#define START_IND_TARGET_ADDRESSING_MODE 0
#define END_IND_TARGET_ADDRESSING_MODE 1
#define START_IND_FUNCT 12
#define END_IND_FUNCT 15
#define START_IND_LABEL 0
#define END_IND_LABEL 15
#define MAX_NUM_LENGTH 10
#define LABEL_WASNT_INITIALIZED_ERROR 15
#define ENTRY_AND_EXTERNAL_ERROR 16
#define SECOND_CHAR_INDEX 1


/*
param: *file_ptr - FILE pointer to assembly file after macros are inserted.
param: *label_array - array of label structs
param: label_array_index - length of label array
param: *data_array - array of data structs
param data_array_index - length of data array
param: IC - IC variable (instruction counter)
param: DC - DC variable (data counter)
param: file_name[] - string of the name of the file
return: 1 if success 0 otherwise
second_pass converts instructions to "special base" and inserts them to .ob file.
the function also creates .ent and .ext files.
if an error is found then the function wont create files.
*/
int second_pass(FILE *file_ptr,label *label_array,int label_array_length,data *data_array,int data_array_length,int IC, int DC, char file_name[]);

#endif