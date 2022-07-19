#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "globals.h"
#include "utils.h"
#include "line_check.h"

/*  boolean functions  */
int is_lable(char word[]);
int is_data(char word[]);
int is_string(char word[]);
int is_entry(char word[]);
int is_extern(char word[]);
int missing_parameter(int *i, char line[]);
int text_after_end(int *i, char line[]);
int label_exist_without_extern (char word[], label arr[], int arr_last);

/*  check func, return num of error msg or 0 if there is no error  */
int no_parameters_command_check(char line[], int line_ind);
int one_parameters_command_check(char word[], char line[], int line_ind);
int two_parameters_command_check(char word[], char line[], int line_ind);
int llegal_data(int *i, char line[]);
int llegal_string(int *i, char line[]);
int llegal_entry_and_extern(int *i, char line[],int is_extern, label arr[], int arr_last);

/*this func returns num of error, or 0 if there is no error.*/
int line_check(char line[],int line_num, label* label_array, int arr_last){
	char word[MAX_LABEL_LENGTH];
	int line_ind = 0;
	
	if(!text_after_end(&line_ind, line))		/*empty line*/
		return 0;
	read_word(&line_ind, line, word);
	if(word[0]==';')            	/*comment line*/
		return 0;
	if(is_label(word)){
		word[strlen(word)-1] = 0;	/*to don't check the ':' char*/
		if(!llegal_label_name(word))
			return ILLEGAL_LABEL_NAME_ERROR;
		if(in_array_label(word, label_array, arr_last) != -1)
			return LABEL_EXIST_ERROR;
		if(missing_parameter(&line_ind, line))    /*nothing after label*/
			return MISSING_OPERAND_ERROR;
		read_word(&line_ind, line, word);
	}
	if(is_data(word)){
		if(missing_parameter(&line_ind, line))    
			return MISSING_OPERAND_ERROR;
		if(line[line_ind]==COMMA)         /*comma after .data*/
			return ILLEGAL_COMMA_ERROR;
		return llegal_data(&line_ind, line);	
	}
	if(is_string(word)){
		if(missing_parameter(&line_ind, line))    
			return MISSING_OPERAND_ERROR;
		return llegal_string(&line_ind, line);
	}
	if(is_entry(word) || is_extern(word)){
		if(missing_parameter(&line_ind, line))    
			return MISSING_OPERAND_ERROR;
		if(line[line_ind]==COMMA)         /*comma after .entry/.extern*/
			return ILLEGAL_COMMA_ERROR;
		return llegal_entry_and_extern(&line_ind, line, is_extern(word), label_array, arr_last);
	}
	if(no_parameters_command(word))
		return no_parameters_command_check(line, line_ind);
	if(one_parameters_command(word))
		return one_parameters_command_check(word, line, line_ind);
	if(two_parameters_command(word)) 
		return two_parameters_command_check(word, line, line_ind);
	return UNDEFINE_COMMAND_NAME_ERROR;
}

/*returns True if there was an error*/ 
int is_error(char line[], FILE* assembly, label* label_array, int arr_last,int line_num){
	char for_skip;
	int error_num;
	if(strchr(line, '\n') == NULL && !feof(assembly)){     /*line longer then 80, max line length allowed*/
		do{	/* skip leftovers */
			for_skip=fgetc(assembly);
		}
		while(for_skip!='\n' && for_skip!=EOF);
		print_msg(LINE_TOO_LONG_ERROR,line_num);
		return TRUE;
	}
	error_num = line_check(line, line_num, label_array, arr_last);
	if(error_num){
		print_msg(error_num,line_num);
		return TRUE;
	}
	return FALSE;
}


/*returns TRUE if missing parameter*/
int missing_parameter(int *i, char line[]){
	*i = skip_white_char(*i, line);
	if(end_of_line(*i, line))
			return TRUE;
	return FALSE;
}


/*returns TRUE if ther is text after end*/
int text_after_end(int *i, char line[]){
	*i = skip_white_char(*i, line);
	if(end_of_line(*i, line))
			return FALSE;
	return TRUE;
}


/*returns TRUE if label in array without extern feature*/
int label_exist_without_extern (char word[], label arr[], int arr_last){
	int i;
	for(i=0;i<=arr_last;i++){
		if(strcmp(word, arr[i].symbol)==0 && !(arr[i].is_external))
			return TRUE;
	}
	return FALSE;
}

/*checks no parameters commands- returns num of error or 0 if ther is to error*/
int no_parameters_command_check(char line[], int line_ind)
{
	if(text_after_end(&line_ind, line))
			return TEXT_AFTER_END_ERROR;
		return 0;
}

/*checks one parameter commands- returns num of error or 0 if ther is to error*/
int one_parameters_command_check(char word[], char line[], int line_ind){
	char operand[MAX_LABEL_LENGTH];
	if(missing_parameter(&line_ind, line))    
		return MISSING_OPERAND_ERROR;
	read_word(&line_ind, line, operand);
	if(strcmp(word, "prn")==0){             /*only one operand command can get all types of addresing mode*/
		if(!is_immediate(operand) && !llegal_label_name(operand) && 
					!is_index_addressing_mode(operand) && !is_register(operand))
			return ILLEGAL_OPERAND_ERROR;
		if(text_after_end(&line_ind, line))
			return TEXT_AFTER_END_ERROR;
		return 0;
	}
	if(strcmp(word, "jump")==0 || strcmp(word, "jump")==0 || strcmp(word, "jump")==0){  /*one operand commands thet get only direct and index addresing mode*/
		if(is_immediate(operand) || is_register(operand))
			return INVALID_OPERAND_TYPE_ERROR;
		if(!llegal_label_name(operand) && !is_index_addressing_mode(operand))
			return ILLEGAL_OPERAND_ERROR;
		if(text_after_end(&line_ind, line))
			return TEXT_AFTER_END_ERROR;
		return 0;
	}
	else{                                 /*other one operand commands can get all types of addresing mode except immediate*/
		if(is_immediate(operand))
			return INVALID_OPERAND_TYPE_ERROR;
		if(!llegal_label_name(operand) && !is_index_addressing_mode(operand) && 
														!is_register(operand))
			return ILLEGAL_OPERAND_ERROR;
		if(text_after_end(&line_ind, line))
			return TEXT_AFTER_END_ERROR;
		}
	return 0;
}

/*checks two parameters commands- returns num of error or 0 if ther is to error*/
int two_parameters_command_check(char word[], char line[], int line_ind){
	char first_operand[MAX_LABEL_LENGTH];
	char second_operand[MAX_LABEL_LENGTH];
	int operand_ind = 0;

	memset(first_operand,0,MAX_LABEL_LENGTH);
	memset(second_operand,0,MAX_LABEL_LENGTH);
	if(missing_parameter(&line_ind, line))    
		return MISSING_OPERAND_ERROR;
	while(!end_of_line(line_ind, line) && line[line_ind]!=COMMA && !isspace(line[line_ind])){  /*get first operand*/
		first_operand[operand_ind] = line[line_ind];
		operand_ind++;
		if(operand_ind>MAX_LABEL_LENGTH)
			return ILLEGAL_OPERAND_ERROR;
		line_ind++;
	}
	line_ind = skip_white_char(line_ind, line);
	if(line[line_ind]!=COMMA)					/*there is no comma*/
		return MISSING_COMMA_ERROR;
	line_ind = skip_white_char(++line_ind, line);		
	if(line[line_ind]==COMMA)					/*there is too many commas*/
		return MULTIPLE_COMMAS_ERROR ;
	if(end_of_line(line_ind, line))				/*there is no second operand*/
		return MISSING_OPERAND_ERROR;
	read_word(&line_ind, line, second_operand);		  /*get second operand*/
	if(strcmp(word, "lea")==0){ 		 /*only two operands command can get only direct and index addresing mode for first operand*/
		if(is_immediate(first_operand) || is_register(first_operand))
			return INVALID_OPERAND_TYPE_ERROR;
		if(!llegal_label_name(first_operand) && !is_index_addressing_mode(first_operand))
			return ILLEGAL_OPERAND_ERROR;
	}
	else{					/*other two operand commands can get all types of addresing mode for first operand*/
		if(!is_immediate(first_operand) && !llegal_label_name(first_operand) && 
					!is_index_addressing_mode(first_operand) && !is_register(first_operand))
			return ILLEGAL_OPERAND_ERROR;
	}
	if(strcmp(word, "cmp")==0){ 		 /*only two operands command can get all types of addresing mode for second operand*/
		if(!is_immediate(second_operand) && !llegal_label_name(second_operand) && 
					!is_index_addressing_mode(second_operand) && !is_register(second_operand))
			return ILLEGAL_OPERAND_ERROR;
	}
	else{					/*other two operand commands can get all types of addresing mode except immediate for second operand*/
		if(is_immediate(second_operand))
			return INVALID_OPERAND_TYPE_ERROR;
		if(!llegal_label_name(second_operand) && !is_index_addressing_mode(second_operand) &&
									 !is_register(second_operand))
			return ILLEGAL_OPERAND_ERROR;
	}
	if(text_after_end(&line_ind, line))
			return TEXT_AFTER_END_ERROR;
	return 0;		
}

/*checks if data is legal- returns num of error or 0 if ther is to error*/
int llegal_data(int *i, char line[]){
	int found_comma=TRUE;
	int sign_on=FALSE;         /*to insure there is number after +/-*/
	while(!end_of_line(*i, line)){
		if(isdigit(line[*i])){
			if(!found_comma)
				return MISSING_COMMA_ERROR;
			sign_on=FALSE;
			(*i)++;
			if(!end_of_line(*i, line) && !isdigit(line[*i]))   
				found_comma=FALSE;
		}	
		else if(found_comma){
			if(line[*i]==COMMA)
				return MULTIPLE_COMMAS_ERROR ;
			else if(!sign_on && (line[*i]=='-' || line[*i]=='+')){
				sign_on=TRUE;
				(*i)++;
			}
			else
				return ILLEGAL_DATA_ERROR;
		}
		else if(line[*i]==COMMA){
			if(sign_on)
				return ILLEGAL_DATA_ERROR;
			found_comma=TRUE;
			(*i)++;
			*i = skip_white_char(*i, line);
		}
		else if(isspace(line[*i])){
			if(sign_on)
				return ILLEGAL_DATA_ERROR;
			*i = skip_white_char(*i, line);
			found_comma=FALSE;
		}
		else
			return ILLEGAL_DATA_ERROR;
	}
	return 0;
}


/*checks if string is legal- returns num of error or 0 if ther is to error*/
int llegal_string(int *i, char line[]){
	if(line[*i] == ','){
		return ILLEGAL_COMMA_ERROR;
	}
	if(line[*i]!='"')
		return ILLEGAL_STRING_ERROR;
	(*i)++;
	while(!end_of_line(*i, line) && line[*i]!='"')
		(*i)++;
	if(end_of_line(*i, line))      /*there is no second '"' to close string*/
		return ILLEGAL_STRING_ERROR;
	else
		(*i)++;
	if(text_after_end(i, line))      
		return TEXT_AFTER_END_ERROR;
	return 0;
}


/*checks if entry and extern are legal- returns num of error or 0 if ther is to error*/
int llegal_entry_and_extern(int *i, char line[],int is_extern, label arr[], int arr_last){
	char word[MAX_LABEL_LENGTH];
	read_word(i, line, word);
	if(!llegal_label_name(word))
		return ILLEGAL_OPERAND_ERROR;
	if(text_after_end(i, line))    
		return TEXT_AFTER_END_ERROR;
	if(is_extern && label_exist_without_extern (word, arr, arr_last))
		return LABEL_WITHOUT_EXTERN_ERROR;
	return 0;
}
