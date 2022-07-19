#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "utils.h"
#include "second_pass.h"

/*returns operand's addressing mode */
int addressing_mode(char operand[], int operand_is_index){
	if(operand_is_index)
		return INDEX_ADDRESSING_MODE;
	else if(is_register(operand))
		return REGISTER_ADDRESSING_MODE;
	else if(is_immediate(operand))
		return IMMEDIATE_ADDRESSING_MODE;
	return DIRECT_ADDRESSING_MODE;
}

/* sets the binary word array to 0 */
void reset_bin_array(int bin_code[]){
	int i;
	for(i=0;i<WORD_SIZE;i++)
		bin_code[i]=0;
}

/*removes first char*/
void remove_first_char(char operand[], char sub_operand[]){
	memset(sub_operand,0,MAX_NUM_LENGTH);
	memcpy(sub_operand,&operand[SECOND_CHAR_INDEX],strlen(operand));
	sub_operand[strlen(operand)] = '\0';
}

/* returns TRUE if label operands in label array. FALSE otherwise */
int is_valid_label_operands(char *split_row[], int split_row_length,int command_index,label *label_array, int label_array_length){
    int i,label_index;
    for(i = command_index + 1; i < split_row_length; i++){
        if(!(is_register(split_row[i]) || is_immediate(split_row[i]))){
            label_index = in_array_label(split_row[i],label_array,label_array_length);
        }
        if(label_index == -1){
            return FALSE;
		}
    }
    return TRUE;
}

/* changes the values of first_operand_is_index_bool and second_operand_is_index_bool to TRUE if they are of type index addressing */
void assign_index_addressing_booleans(int *first_operand_is_index_bool,int *second_operand_is_index_bool,char *split_row[],int split_row_length,int command_index){
    *first_operand_is_index_bool = FALSE;
    *second_operand_is_index_bool = FALSE;
    if(split_row_length - command_index > 1){ /* if atleast 1 operand */
        if(strchr(split_row[command_index + 1],'[') != NULL){ /* command_index + 1 is the first operand index in split_row */
            *first_operand_is_index_bool = TRUE;
        }
        if(split_row_length - command_index == 3){ /* if 2 operands */
            if(strchr(split_row[command_index + 2],'[') != NULL){ /* command_index + 2 is the second operand index in split_row */
                *second_operand_is_index_bool = TRUE;
            }
        }
    }
}

/*add to the ob file all lines of commands (opcode's line, funct and register's line, and line of immediate/label's base&offset.
  add to the ext file base&offeset of external labels*/
void add_to_ext_ob_files(FILE *ob_file_pointer, FILE *ext_file_pointer,label *label_array, int end_index_label_array, char *split_row[], 
			int num_of_elements, int first_operand_is_index,int second_operand_is_index, int command_ind, int *IC, int *extern_flag){
	int bin_code[WORD_SIZE];
	int i,source_register,target_register,source_addressing_mode,target_addressing_mode,opcode,funct,ind_of_label;
	char *sub_operand=NULL;
	source_register = target_register = source_addressing_mode = target_addressing_mode = 0;
	sub_operand = malloc(MAX_NUM_LENGTH*sizeof(char));

	/*get opcode and funct*/
	for(i=0;i<AMOUNT_OF_COMMANDS;i++){
		if(strcmp(split_row[command_ind], command_list[i].name) == 0){
			opcode = command_list[i].opcode;
			funct = command_list[i].funct;
			break;
		}
	}
	
	/*first line*/
	reset_bin_array(bin_code);
	bin_code[A] = 1;
	bin_code[opcode] = 1;
	fprintf(ob_file_pointer, "%04d %s\n", *IC, change_base(bin_code));
	(*IC)++;
	
	/*second line*/
	reset_bin_array(bin_code);
	bin_code[A] = 1;	
	if(one_parameters_command(split_row[command_ind])){
		/*if there is a register in one operand command it will be in last index*/
		if(is_register(split_row[num_of_elements - 1])){
			remove_first_char(split_row[num_of_elements - 1], sub_operand);		/*to remove the 'r'*/
			target_register = atoi(sub_operand);
		}
		target_addressing_mode = addressing_mode(split_row[command_ind + 1], first_operand_is_index);
	}
	if(two_parameters_command(split_row[command_ind])){
		/*if first operand in index addressing mode, index after command will contain the label and one after the register*/
		if(first_operand_is_index){
			remove_first_char(split_row[command_ind + 2], sub_operand);		/*to remove the 'r'*/
			source_register = atoi(sub_operand);
		}
		/*if first operand is a register it will be in index after command*/
		else if(is_register(split_row[command_ind + 1])){
			remove_first_char(split_row[command_ind + 1], sub_operand);		/*to remove the 'r'*/
			source_register = atoi(sub_operand);
		}
		/*if there is a register in the second operand it will be in last index*/
		if(is_register(split_row[num_of_elements - 1])){
			remove_first_char(split_row[num_of_elements - 1], sub_operand);		/*to remove the 'r'*/
			target_register = atoi(sub_operand);
		}
		source_addressing_mode = addressing_mode(split_row[command_ind + 1], first_operand_is_index);
		target_addressing_mode = addressing_mode(split_row[num_of_elements - 1], second_operand_is_index);
	}
	decimal_to_binary_pos(source_register,bin_code,START_IND_SOURCE_REGISTER,END_IND_SOURCE_REGISTER);
	decimal_to_binary_pos(target_register,bin_code,START_IND_TARGET_REGISTER,END_IND_TARGET_REGISTER);
	decimal_to_binary_pos(source_addressing_mode,bin_code,START_IND_SOURCE_ADDRESSING_MODE,END_IND_SOURCE_ADDRESSING_MODE);
	decimal_to_binary_pos(target_addressing_mode,bin_code,START_IND_TARGET_ADDRESSING_MODE,END_IND_TARGET_ADDRESSING_MODE);
	decimal_to_binary_pos(funct,bin_code,START_IND_FUNCT,END_IND_FUNCT);
	if(!no_parameters_command(split_row[command_ind])){
		fprintf(ob_file_pointer, "%04d %s\n", *IC, change_base(bin_code));
		(*IC)++;
	}
	for(i=command_ind+1;i<num_of_elements;i++){
		if(!is_register(split_row[i])){
			reset_bin_array(bin_code);
			if(is_immediate(split_row[i])){
				remove_first_char(split_row[i],sub_operand);  		/*to remove the '#'*/
				decimal_to_binary(atoi(sub_operand),bin_code);
				bin_code[A] = 1;
			}
			else{			/*this is a label*/
				ind_of_label = in_array_label(split_row[i], label_array,end_index_label_array);
				bin_code[R] = 1;
				decimal_to_binary_pos(label_array[ind_of_label].base,bin_code,START_IND_LABEL,END_IND_LABEL);
				if(label_array[ind_of_label].is_external){
					*extern_flag = TRUE;
					fprintf(ext_file_pointer, "%s BASE %d\n", label_array[ind_of_label].symbol, *IC);
					bin_code[R] = 0;
					bin_code[E] = 1;
				}
				fprintf(ob_file_pointer, "%04d %s\n", *IC, change_base(bin_code));
				(*IC)++;
				reset_bin_array(bin_code);
				bin_code[R] = 1;
				decimal_to_binary_pos(label_array[ind_of_label].offset,bin_code,START_IND_LABEL,END_IND_LABEL);
				if(label_array[ind_of_label].is_external){
					fprintf(ext_file_pointer, "%s OFFSET %d\n\n", label_array[ind_of_label].symbol, *IC);
					bin_code[R] = 0;
					bin_code[E] = 1;
				}	
			}
			fprintf(ob_file_pointer, "%04d %s\n", *IC, change_base(bin_code));
			(*IC)++;
		}
	}
	free(sub_operand);
}

/* adds label to .ent file */
void add_to_ent(FILE *ent_file_pointer,label *label_array,int label_index){
    fprintf(ent_file_pointer,"%s,%d,%d\n",label_array[label_index].symbol,label_array[label_index].base,label_array[label_index].offset);
}

/* adds data in special base to .ob file */
void add_data_to_ob(FILE *ob_file_pointer, data *data_array, int data_array_length,int IC,int *object_file_bool){
    int i,j;
    int *data_int_array;
    int binary_data[BINARY_CODE_LENGTH];
    for(i = 0;i < data_array_length; i++){
        data_int_array = data_array[i].num_list;
        for(j = 0; j < data_array[i].num_list_size; j++){
			reset_bin_array(binary_data);
            decimal_to_binary(data_int_array[j],binary_data);
            binary_data[A] = 1;
            fprintf(ob_file_pointer,"%04d %s\n",IC,change_base(binary_data));
            IC++;
        }
		free(data_array[i].num_list);
    }
    if(i != 0){
        *object_file_bool = 1;
    }
}

/* removes file with argument file name  */
void remove_file(char name[],char ending[]){
    int len;
    char *new_string;
    len = strlen(name) + strlen(ending);
    new_string = malloc(sizeof(char) * len);
    strcpy(new_string,name);
    strcat(new_string,ending);
    remove(new_string);
    free(new_string);
}

/*
second_pass converts instructions to "special base" and inserts them to .ob file.
the function also creates .ent and .ext files.
if an error is found then the function wont create files.
*/
int second_pass(FILE *file_ptr,label *label_array,int label_array_length,data *data_array,int data_array_length,int IC, int DC, char file_name[]){
    int command_index,label_index,split_row_length,entry_file_bool,extern_file_bool,object_file_bool,line_count;
    int first_operand_is_index_bool,second_operand_is_index_bool,error_bool;
    char row[MAX_ROW_SIZE],row_copy[MAX_ROW_SIZE]; /* copy of row because split_command function changes the original string. */
    char *split_row[MAX_ROW_SIZE];
    FILE *ob_file,*ent_file,*ext_file;
    object_file_bool = entry_file_bool = extern_file_bool = error_bool = FALSE;
    line_count = 1;
    ob_file = open_and_name_new_file(file_name,".ob");
    ent_file = open_and_name_new_file(file_name,".ent");
    ext_file = open_and_name_new_file(file_name,".ext");
    fprintf(ob_file, "\t%d %d\n", IC - STARTING_IC, DC);
    IC = STARTING_IC;
    while((fgets(row, MAX_ROW_SIZE, file_ptr)!= NULL)){
        strcpy(row_copy,row);
        memset(split_row,0,MAX_ROW_SIZE * sizeof(char *));
        command_index = 0; /* command_index will be 1 if theres a label in row. 0 otherwise. */
        split_row_length = split_command(row,split_row,FIRST_PASS_COMMAND_DELIMITER);
        
        if(split_row_length > 0){ /* if not an empty row */
            if(is_label(split_row[command_index])){
                command_index++;
            }
            if(!(is_data(split_row[command_index]) || is_string(split_row[command_index]) || is_extern(split_row[command_index]) || split_row[command_index][0] == ';')){
                if(is_entry(split_row[command_index])){
                    label_index = in_array_label(split_row[command_index + 1],label_array,label_array_length);
                    if(label_index == -1){ /* if label operand not in label array. */
                        print_msg(LABEL_WASNT_INITIALIZED_ERROR,line_count);
						error_bool = TRUE;
                    }
                    else{
                        if(label_array[label_index].is_external){ /* if its external and entry -> error */
                            print_msg(ENTRY_AND_EXTERNAL_ERROR,line_count);
							error_bool = TRUE;
                        }
                        else{
                            label_array[label_index].is_entry = TRUE;
                            entry_file_bool = TRUE;
                            add_to_ent(ent_file,label_array,label_index);
                        }
                    }
                }
                else{ /* its a instruction command */
                    assign_index_addressing_booleans(&first_operand_is_index_bool,&second_operand_is_index_bool,split_row,split_row_length,command_index);
                    if((first_operand_is_index_bool || second_operand_is_index_bool)){ /* if there is index addressing -> split again with "][" delimiter. */
                        split_row_length = split_command(row_copy,split_row,SECOND_PASS_COMMAND_DELIMITER);
                    }
                    if(!is_valid_label_operands(split_row,split_row_length,command_index,label_array,label_array_length)){
                        print_msg(LABEL_WASNT_INITIALIZED_ERROR,line_count);
						error_bool = TRUE;
                    }
                    else{
                        object_file_bool = TRUE;
                        add_to_ext_ob_files(ob_file,ext_file,label_array,label_array_length,split_row,split_row_length,first_operand_is_index_bool,second_operand_is_index_bool,command_index,&IC,&extern_file_bool);
                    }
                }
            }
            line_count++;
        }
    }

    add_data_to_ob(ob_file,data_array,data_array_length,IC,&object_file_bool); /* adds all data to object file  */
    fclose(ob_file);
    fclose(ext_file);
    fclose(ent_file);
	/* delete files if empty */
    if(!object_file_bool || error_bool){
		remove_file(file_name,".ob");
    }
    if(!entry_file_bool || error_bool){
		remove_file(file_name,".ent");
    }
    if(!extern_file_bool || error_bool){
		remove_file(file_name,".ext");
    }
	return !error_bool;
}
