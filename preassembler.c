#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "macro.h"
#include "globals.h"
#include "utils.h"
#include "preassembler.h"

int start_macro(char word[]);
int end_macro(char word[]);
const char* get_content(char word[], struct macro arr[]);
void if_line_to_long(char line[], FILE* assembly);
int in_array_macro(char word[], macro arr[], int arr_last);


/* This func inserts the macros to the file */
void pre_assembler(FILE* assembly, char file_name[]){
	FILE *for_assembler_file;
	int in_macro = FALSE;
	char line[MAX_ROW_SIZE+ADD_TO_LINE_LENGTH]; /*we adding to length so if the line is to long we will be able to discover it in the finding error phase*/
	char first_word[MAX_MACRO_NAME_LENGTH];              
	int line_ind;       
	macro* macro_array=NULL; 
	int extension_num=1;   /*when macro array out of space we will realloc, this is counter for how many times*/       
	int macro_array_last=0; 
	for_assembler_file = open_and_name_new_file(file_name, ".am");
	macro_array = malloc(DYNAMIC_ARRAY_INITIAL_SIZE*sizeof(macro));
	
	while (fgets(line, MAX_ROW_SIZE, assembly)!= NULL){
		line_ind=0;
		if_line_to_long(line, assembly);		/* skip leftovers if line to long and fix end of the line for copy*/
		line_ind = skip_white_char(line_ind, line);
		if(end_of_line(line_ind, line)){		/*empty line*/
			fprintf(for_assembler_file, "\n");
		}
		read_word(&line_ind, line, first_word);
		if(in_macro){
			if(end_macro(first_word)){
				in_macro = FALSE;
				macro_array_last++;
				if(macro_array_last==(extension_num*DYNAMIC_ARRAY_INITIAL_SIZE)){
					extension_num++;
					macro_array = realloc(macro_array, (extension_num*DYNAMIC_ARRAY_INITIAL_SIZE*sizeof(macro)));
				}
			}			
			else{
				strcat(macro_array[macro_array_last].content, line);   /*add to macro content*/
			}
		}

		else if(start_macro(first_word))    {
			in_macro = TRUE;
			read_word(&line_ind, line, first_word);      /* get macro name */
			strcpy(macro_array[macro_array_last].name, first_word);   /*set name to macro*/
			strcpy(macro_array[macro_array_last].content, "");
		}
		else if(in_array_macro(first_word, macro_array, macro_array_last)){
			fprintf(for_assembler_file, "%s", get_content(first_word, macro_array));
		}
		else{
			fprintf(for_assembler_file, "%s", line);
		}
	}
	fclose(for_assembler_file);
}

/*returns TRUE if name is allready in macro array*/
int in_array_macro(char word[], macro arr[], int arr_last){
	int i;
	for(i=0;i<=arr_last;i++){
		if(strcmp(word, arr[i].name)==0)
			return TRUE;
	}
	return FALSE;
}

/*skips leftovers if line to long and fixs end of the line for copy*/
void if_line_to_long(char line[], FILE* assembly){
	char for_skip;
	if(strchr(line, '\n') == NULL && !feof(assembly)){
		do{				/* skip leftovers */
			for_skip=fgetc(assembly);
		}
		while(for_skip!='\n' && for_skip!=EOF);
		strcat(line,"\n");
	}
}

/*returns TRUE if word is macro*/
int start_macro(char word[]){ 
	if(strcmp(word, "macro")==0)          /* 0 meens equal*/
		return TRUE;
	return FALSE;  
}

/*returns TRUE if word is endm*/
int end_macro(char word[]){
	if(strcmp(word, "endm")==0)
		return TRUE;
	return FALSE;     
}

/*returns content of macro*/
const char* get_content(char word[], struct macro arr[]){
	int i=0;
	while(strcmp(word, arr[i].name) != 0)
		i++;
	return arr[i].content;	                            
}
