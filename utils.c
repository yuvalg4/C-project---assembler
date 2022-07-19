#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "globals.h"
#include "utils.h"

/*returns true if its end of the line*/
int end_of_line (int i, char row[]){
	return i == strlen(row) || row[i] == '\n';
}

/*skips white char*/
int skip_white_char (int i, char row[]){
	if(end_of_line(i, row))
		return i;
	while (isspace(row[i]))
		i++;
	return i;
}

/*reads next word from the row*/
void read_word (int *row_index, char row[], char next_word[]){
	int j=0;
	*row_index = skip_white_char(*row_index, row);
	memset(next_word, 0, MAX_LABEL_LENGTH);;
	while(!end_of_line(*row_index, row) && strchr(WHITE_CHAR, row[*row_index]) == NULL){
		next_word[j++] = row[(*row_index)++];
	}
}

/*returns true if label name is legal-length up to 31, first char is a letter, contain only letters and number, not equal to register or command name..*/
int llegal_label_name(char operand[]){
	int i=0;
	if(strlen(operand)>MAX_LABEL_LENGTH)
		return FALSE;
	if(!isalpha(operand[i]))
		return FALSE;
	while(operand[i]!=0){
		if(!isalnum(operand[i]))
			return FALSE;
		i++;
	}
	for(i=0; i<AMOUNT_OF_COMMANDS; i++){
		if(strcmp(operand, command_list[i].name)==0 || strcmp(operand, registers_list[i].name)==0)
			return FALSE;
	}
	return TRUE;
}

/*returns true if it is a register's name*/
int is_register(char operand[]){
	int i;
	for(i=0; i<AMOUNT_OF_REGISTERS; i++){
		if(strcmp(operand, registers_list[i].name)==0)
			return TRUE;
	}
	return FALSE;
}

/*returns true if it is in index addressing mode- label[r10]..label[r15]*/
int is_index_addressing_mode(char operand[]){
	int registers_list_ind,word_ind,to_check_ind;
	char to_check[MAX_LABEL_LENGTH];
	word_ind = to_check_ind = 0;	
	memset(to_check, 0, MAX_LABEL_LENGTH);
	while(word_ind<strlen(operand) && operand[word_ind]!='['){
		to_check[to_check_ind] = operand[word_ind];
		to_check_ind++;
		word_ind++;
	}
	if(!llegal_label_name(to_check))
		return FALSE;
	if(operand[word_ind]!='[')
		return FALSE;
	word_ind++;
	to_check_ind = 0;
	memset(to_check, 0, MAX_LABEL_LENGTH);
	while(word_ind<strlen(operand) && operand[word_ind]!=']'){
		to_check[to_check_ind] = operand[word_ind];
		to_check_ind++;
		word_ind++;
	}
	if(!is_register(to_check))                   
		return FALSE;
	for(registers_list_ind=0; registers_list_ind<FIRST_REGISTER_ALLOWED; registers_list_ind++){     /*can be only registers 10 to 15 */
		if(strcmp(to_check, registers_list[registers_list_ind].name)==0)
			return FALSE;
	}
	if(operand[word_ind]!=']')
		return FALSE;
	word_ind++;
	if(word_ind!=strlen(operand))              /*something after ']'*/
		return FALSE;
	return TRUE;	
}


/*returns true if it is an immediate*/
int is_immediate(char operand[]){
	int i=0;
	if(operand[i++]!='#')
		return FALSE;
	if(strlen(operand) == 1)         /*only #, without number after it*/
		return FALSE;
	if(operand[i]=='-' || operand[i]=='+')
		i++;
	while(i<strlen(operand)){
		if(!isdigit(operand[i]))
			return FALSE;
		i++;
	}
	return TRUE;
}


/*returns true if it is a no parameters command*/
int no_parameters_command(char command[]){
	int i;
	for(i=0; i<AMOUNT_OF_COMMANDS; i++){
		if(strcmp(command, command_list[i].name)==0 && command_list[i].num_operand == NO_PARAMETERS)
			return TRUE;
	}
	return FALSE;
}

/*returns true if it is a one parameter command*/
int one_parameters_command(char command[]){
	int i;
	for(i=0; i<AMOUNT_OF_COMMANDS; i++){
		if(strcmp(command, command_list[i].name)==0 && command_list[i].num_operand == ONE_PARAMETER)
			return TRUE;
	}
	return FALSE;
}

/*returns true if it is a two parameters command*/
int two_parameters_command(char command[]){
	int i;
	for(i=0; i<AMOUNT_OF_COMMANDS; i++){
		if(strcmp(command, command_list[i].name)==0 && command_list[i].num_operand == TWO_PARAMETERS)
			return TRUE;
	}
	return FALSE;
}

/*returns true if it is .data*/
int is_data(char command[]){
	return (strcmp(command, DATA)==0);
}

/*returns true if it is .string*/
int is_string(char command[]){
	return (strcmp(command, STRING)==0);
}

/*returns true if it is .entry*/
int is_entry(char command[]){
	return (strcmp(command, ENTRY)==0);
}

/*returns true if it is .extern*/
int is_extern(char command[]){
	return (strcmp(command, EXTERN)==0);
}

/*returns true if it is a labet (end with ':')*/
int is_label(char operand[]){
	return (operand[strlen(operand)-1] == ':');
}

/* removes the last char of a given string */
void remove_last_char(char *word){
	word[strlen(word) - 1] = '\0';
}

/* converts binary code to hex at a given range and stores it in after_base_change */
void bin_to_hex(int bin_code[],int start,int finish,char *after_base_change,int array_index){
    int i,sum;
    sum = 0;
    i = DIGITS_BASE_CHANGE - 1;
    for(;start > finish;start--){ /* adds to sum 2 to the power of i = 19,18,17,16 */
        sum = sum + bin_code[start] * pow(BINARY_BASE,i--);
    }
    if(sum >= ASCII_OFFSET){ /* if sum is bigger or equal to 10 we turn to letters a-f */
        after_base_change[array_index] = sum - ASCII_OFFSET + 'a';
    }
    else{
        after_base_change[array_index] = sum + '0';
    }
}

/* changes binary code to special base */
char *change_base(int bin_code[]){
    char *after_base_change = malloc(BASE_CHANGE_SIZE * sizeof(char)); /*This array will contain the code in new base*/
    int i,j,k;
    j = k = 0;
    for(i = WORD_SIZE-1; i > 0 ; i = i - DIGITS_BASE_CHANGE){
        after_base_change[j] = A_ASCII + k++; /*Adds beginning letter from A to E*/
        j++;
        bin_to_hex(bin_code,i,(i - DIGITS_BASE_CHANGE),after_base_change,j++);
        if(i != DIGITS_BASE_CHANGE - 1){
            after_base_change[j++] = '-';
        }
    }
    return &(after_base_change[0]);
}

/* inserts binary representation of decimal number to empty array. */
void decimal_to_binary(int input_num, int bin_array[]){
	int shift, current, bit_index;
	bit_index = 0;
	for (shift = 0 ; shift < INT_BITS_SIZE ; shift++){
		current = input_num >> shift;
		if (current & 1)
			bin_array[bit_index] = 1;
		else
			bin_array[bit_index] = 0;
		bit_index++;
	}
}

/* converts positive decimal numbers to binary. inserts in array at a given range. */
void decimal_to_binary_pos(int num,int bin_array[],int start, int end){
	int i;
	for(i=start ; i <= end ;i++)
	{
		bin_array[i]=num%2;
		num=num/2;
	}
}

/* splits the row into subarrays and stores them in *split_row[]. returns length of split_row */
int split_command(char row[],char *split_row[], char delimt[]){
    int i = 0;
    char *p = strtok(row,delimt);
    while(p != NULL){
        split_row[i] = p;
        i++;
        p = strtok(NULL,delimt);
    }
    return i; /* returns number of elements */
}

/*open and returns pointer to a new file in given type ending, for ex. file_name = x, file_type = .am => file name will be x.am*/
FILE *open_and_name_new_file(char file_name[], char file_type[]){
	FILE *file;
	char* new_file_name=NULL;
	new_file_name = malloc(strlen(file_name)+END_FILE_EXPRESSION_LENGTH);
	strcpy(new_file_name, file_name);
	strcat(new_file_name, file_type);
	file = fopen (new_file_name, "w");
	if(file == NULL){  
		free(new_file_name);  
      		return NULL;             		
   	}
	free(new_file_name);
	return file;
}

/*returns index of element or -1 if it's not in array*/
int in_array_label(char label_name[], label arr[], int end_index){
	int i;
	for(i = 0;i < end_index;i++){
		if(strcmp(label_name, arr[i].symbol)==0)
			return i; /* returns index of element */
	}
	return NOT_IN_ARRAY;
}

/*prints error mgs in format- ERROR! in line (nember of line): (type of error).\n */
void print_msg(int msg_num, int row_num){
	char msg[MSG_LENGTH];
	switch(msg_num){
		case 1:
			strcpy(msg, "Line too long (more then 80 chars)");
			break;
		case 2:
			strcpy(msg, "Missing operand");
			break;
		case 3:
			strcpy(msg, "Illegal data");
			break;
		case 4:
			strcpy(msg, "Invalid operand - operand type incompatible");
			break;
		case 5:
			strcpy(msg, "Multiple consecutive commas");
			break;
		case 6:
			strcpy(msg, "Missing comma");
			break;
		case 7:
			strcpy(msg, "Extraneous text after end of command");
			break;
		case 8:
			strcpy(msg, "Illegal operand");
			break;
		case 9:
			strcpy(msg, "Illegal comma");
			break;
		case 10:
			strcpy(msg, "Undefined command name");
			break;
		case 11:
			strcpy(msg, "Illegal label name");
			break;
		case 12:
			strcpy(msg, "Label already exist");
			break;
		case 13:
			strcpy(msg, "Illegal string");
			break;
		case 14:
			strcpy(msg, "Label already exist without extern feature");
			break;
		case 15:
			strcpy(msg, "Label was not initialized");
			break;
		case 16:
			strcpy(msg, "Label can not be defined as entry and as external");
			break;
		case 17:
			strcpy(msg, "File too long! (Instruction count bigger than 8192).");
			break;
		case 18:
			strcpy(msg, "Int inserted not in the range of 16 bits");
			break;
	}
	printf("ERROR! in line %d: %s\n", row_num, msg);
}

/* constant array of registers structs. the array contains all the valid registers in the assignment */
const struct registers registers_list[AMOUNT_OF_REGISTERS] = {
    {"r0",0},
    {"r1",1},
    {"r2",2},
    {"r3",3},
    {"r4",4},
    {"r5",5},
    {"r6",6},
    {"r7",7},
    {"r8",8},
    {"r9",9},
    {"r10",10},
    {"r11",11},
    {"r12",12},
    {"r13",13},
    {"r14",14},
    {"r15",15}
};

/*
constant array of command_opcode_funct structs. the array contains all the valid commands and their
corresponding funct and opcode in the assignment
*/
const struct command_opcode_funct command_list[AMOUNT_OF_COMMANDS] = {
    {"mov",0,0,2},
    {"cmp",0,1,2},
    {"add",10,2,2},
    {"sub",11,2,2},
    {"lea",0,4,2},
    {"clr",10,5,1},
    {"not",11,5,1},
    {"inc",12,5,1},
    {"dec",13,5,1},
    {"jmp",10,9,1},
    {"bne",11,9,1},
    {"jsr",12,9,1},
    {"red",0,12,1},
    {"prn",0,13,1},
    {"rts",0,14,0},
    {"stop",0,15,0}
};