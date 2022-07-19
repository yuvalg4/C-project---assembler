#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "utils.h"
#include "line_check.h"
#include "first_pass.h"

/* returns how many rows of binary code a command needs. */
int how_many_rows(char *split_row[], int num_of_elements, int command_index){
	int num_of_lines = BASE_NUM_OF_LINES;
	if(no_parameters_command(split_row[command_index]))
		return num_of_lines;
        
	num_of_lines++; /*all other commands has at least 2 lines*/
	for(command_index++ ; command_index<num_of_elements; command_index++){	/*command_ind++ to skip command*/
		if(is_immediate(split_row[command_index]))
			num_of_lines++;
		else if(!is_register(split_row[command_index]))	/*direct or index addressing mode*/
			num_of_lines = num_of_lines + 2;
	}
	return num_of_lines;	
}

/* converts array of string number to int array. returns the int array. */
int * convert_data_to_int_array(int start,int end, char *split_row[],int *error_bool,int line_num){
    int *return_array = malloc(sizeof(int) * (end - start + 1));
    int i,func_bool;
    func_bool = FALSE;
    for(i = start;i < end;i++){
        return_array[i - start] = atoi(split_row[i]);
        if(!func_bool && (return_array[i - start] > BIGGEST_NUMBER || return_array[i - start] < SMALLEST_NUMBER)){
            print_msg(NUMBER_NOT_IN_RANGE_ERROR,line_num);
            func_bool = TRUE;
            *error_bool = TRUE;
        }
    }
    return return_array;
}

/* adds data to the dynamic data array */
void add_data_to_data_array(data *data_array,int *data_array_index,char *split_row[],int start_index,int DC,int end_index,int *error_bool, int line_num){
    data_array[*data_array_index].is_data = TRUE;
    data_array[*data_array_index].is_string = FALSE;
    data_array[*data_array_index].num_list = convert_data_to_int_array(start_index,end_index,split_row,error_bool,line_num);
    data_array[*data_array_index].DC = DC;
    data_array[*data_array_index].num_list_size = end_index - start_index;
}

/* accepts a string and assigns corresponding ascii values to int array. */
void convert_string_to_int_array(char string[],int *string_converted_int_array){
    int i = 0;
    while(string[i] != '\0'){
        *(string_converted_int_array + i) = string[i];
        i++;
    }
    *(string_converted_int_array + i) = 0;
}

/* adds string to the dynamic data array, returns amount of words added to the data array (DC) */
int add_string_to_data_array(data *data_array,int *data_array_index,char row[],int command_index,int DC){
    int DC_increment;
    char *split_row[NUMBER_OF_ELEMENTS_STRING_COMMAND];
    char string[MAX_ROW_SIZE];
    int *string_converted_int_array;
    split_command(row,split_row,STRING_DELIMITER);
    strcpy(string,split_row[STRING_INDEX]);
    string[strlen(split_row[STRING_INDEX])] = '\0';
    string_converted_int_array = malloc(sizeof(int) * ((strlen(string)) + 1)); /* +1 for '\0' */
    convert_string_to_int_array(string,string_converted_int_array);
    DC_increment = strlen(string) + 1;
    data_array[*data_array_index].num_list = string_converted_int_array;
    data_array[*data_array_index].is_data = FALSE;
    data_array[*data_array_index].is_string = TRUE;
    data_array[*data_array_index].num_list_size = DC_increment;
    data_array[*data_array_index].DC = DC;
    return DC_increment;
}

/* adds a label to the dynamic label array */
void add_to_label_array(label *label_array, int *label_array_index,char symbol[],int DC,int is_data,int is_code,int is_entry,int is_external){
    strcpy(label_array[*label_array_index].symbol,symbol);
    label_array[*label_array_index].value = DC;
    label_array[*label_array_index].base = (DC / DIVIDE_BASE_OFFSET) * DIVIDE_BASE_OFFSET;
    label_array[*label_array_index].offset = DC % DIVIDE_BASE_OFFSET;
    label_array[*label_array_index].is_data = is_data;
    label_array[*label_array_index].is_code = is_code;
    label_array[*label_array_index].is_entry = is_entry;
    label_array[*label_array_index].is_external = is_external;
}

/* dynamic array maintenance. if the array is at its maximum size then we realloc */
void label_array_maintenance(label **label_array,int *label_array_index,int *label_extension_num){
    if(*(label_array_index)==(DYNAMIC_ARRAY_INITIAL_SIZE * (*label_extension_num)))
    {
        *label_extension_num = *label_extension_num + 1;
        *label_array = realloc(*label_array, (*label_extension_num * DYNAMIC_ARRAY_INITIAL_SIZE * sizeof(label)));
    }
}

/* dynamic array maintenance. if the array is at its maximum size then we realloc */
void data_array_maintenance(data **data_array,int *data_array_index,int *data_extension_num){
    if(*(data_array_index)==(DYNAMIC_ARRAY_INITIAL_SIZE * (*data_extension_num)))
    {
        *data_extension_num = *data_extension_num + 1;
        *data_array = realloc(*data_array, (*data_extension_num * DYNAMIC_ARRAY_INITIAL_SIZE * sizeof(data)));
    }
}

/* updates all the labels that are of data type. (19. at first pass algorithm)  */
void update_label_array(label *label_array,int label_array_index,int IC){
    int i,new_value;
    for(i = 0;i < label_array_index; i++){
        if(label_array[i].is_data){
            new_value = label_array[i].value + IC;
            label_array[i].value = new_value;
            label_array[i].offset = new_value % DIVIDE_BASE_OFFSET;
            label_array[i].base = (new_value / DIVIDE_BASE_OFFSET) * DIVIDE_BASE_OFFSET;
        }
    }
}

/*the function saves all the labels,data in their corresponding array while updating DC and IC.*/
int first_pass(FILE *file_ptr,label **label_array,int *label_array_index, data **data_array,int *data_array_index,int *IC,int *DC){
    char row[MAX_ROW_SIZE],row_copy[MAX_ROW_SIZE];
    char *split_row[MAX_ROW_SIZE]; /* this array will contain the subarrays after splitting them */
    int is_data_bool,is_string_bool,error_bool,label_bool;
    int num_elements_array,command_index, DC_increment,L;
    int data_extension_num,label_extension_num,line_num; /* when label array out of space we will realloc, this is counter for how many times*/
    data_extension_num = label_extension_num = line_num = 1;
    error_bool = label_bool = FALSE;
    while((fgets(row, MAX_ROW_SIZE, file_ptr)!= NULL)){
        if(is_error(row,file_ptr,*label_array,*label_array_index,line_num)){
            error_bool = TRUE;
        }
        else{ /* if error is found we dont need to keep iterating over the rows. only to look for more errors. */
            memset(split_row,0,sizeof(char *) * MAX_ROW_SIZE); /* reinitializing split_row so we wont have subarrays from different iteration */
            strcpy(row_copy, row);
            label_bool = FALSE; /* is there a label in the row? */
            command_index = 0; /* if label is found then the command will in be index 1. 0 otherwise. */
            
            num_elements_array = split_command(row,split_row,FIRST_PASS_COMMAND_DELIMITER); /* split_row now contains the splitted arrays */
            if(!(num_elements_array == 0 || split_row[command_index][0] == ';')){ /* skip row if found */
                if(is_label(split_row[command_index])){
                    remove_last_char(split_row[command_index]);
                    label_bool = TRUE;
                    command_index++;
                }

                if((is_data_bool = is_data(split_row[command_index])) || (is_string_bool = is_string(split_row[command_index]))){
                    if(label_bool){
                        add_to_label_array(*label_array,label_array_index,split_row[command_index - 1],*DC,TRUE,FALSE,FALSE,FALSE);
                        *(label_array_index) = *(label_array_index) + 1;
                        /* dynamic label array maintenance */
                        label_array_maintenance(label_array,label_array_index,&label_extension_num);
                    }

                    if(is_data_bool){
                        add_data_to_data_array(*data_array,data_array_index,split_row,command_index + 1,*DC,num_elements_array,&error_bool,line_num); /* command_index + 1 is the beggining of the data itself. */
                        DC_increment = num_elements_array - command_index - 1; /* number of elements in split_row minus the label and command (length of the data itself). */
                    }
                    else { /* its a .string */
                        DC_increment = add_string_to_data_array(*data_array,data_array_index,row_copy,command_index,*DC);
                    }

                    *DC = *DC + DC_increment; /* adds to dc the amount of elements added. */
                    *(data_array_index) = *(data_array_index) + 1;

                    /* dynamic data array maintenance */
                    data_array_maintenance(data_array,data_array_index,&data_extension_num);
                }

                else if(is_extern(split_row[command_index])){
                    add_to_label_array(*label_array,label_array_index,split_row[command_index + 1],EXTERNAL_BASE_OFFSET,FALSE,FALSE,FALSE,TRUE);
                    *(label_array_index) = *(label_array_index) + 1;
                    label_array_maintenance(label_array,label_array_index,&label_extension_num);
                }

                /* if is_entry() we dont do anything until the second pass */
                else if(!is_entry(split_row[command_index])){ /* This is a instruction command */
                    if(label_bool){
                        add_to_label_array(*label_array,label_array_index,split_row[command_index - 1],*IC,FALSE,TRUE,FALSE,FALSE);
                        *(label_array_index) = *(label_array_index) + 1;
                        label_array_maintenance(label_array,label_array_index,&label_extension_num);
                    }
                    L = how_many_rows(split_row,num_elements_array,command_index);
                    *IC = *IC + L;
                }
            }
        }
        line_num++;
    }
    if(*IC + *DC > MAX_LINES_IN_FILE){
        print_msg(FILE_TOO_LONG_ERROR,line_num);
        error_bool = TRUE;
    }
    if(error_bool){
        return FALSE;
    }
    update_label_array(*label_array,*label_array_index,*IC);
    return TRUE;

}