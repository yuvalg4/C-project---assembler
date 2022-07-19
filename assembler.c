#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "utils.h"
#include "first_pass.h"
#include "second_pass.h"
#include "preassembler.h"

#define STARTING_DC 0

/* just like strcat but returns a NEW char pointer */
char *concat_string(char s1[],char s2[]){
    int len;
    char *new_string;
    len = strlen(s1) + strlen(s2);
    new_string = malloc(sizeof(char) * len);
    strcpy(new_string,s1);
    strcat(new_string,s2);
    return new_string;
}

int main(int argc, char *argv[]){
    int IC,DC,file_index,first_pass_bool,second_pass_bool;
    char *temp_string;
    FILE *file_for_open,*file_for_assembler;
    label *label_array;
    data *data_array;
    int label_array_index,data_array_index;
    for(file_index = 1; file_index < argc;file_index++){
        IC = STARTING_IC;
        DC = STARTING_DC;
        first_pass_bool = second_pass_bool = FALSE;
        label_array_index = data_array_index = 0;
        label_array = malloc(DYNAMIC_ARRAY_INITIAL_SIZE*sizeof(label));
        data_array = malloc(DYNAMIC_ARRAY_INITIAL_SIZE*sizeof(data));
        temp_string = concat_string(argv[file_index],".as");
        file_for_open = fopen(temp_string,"r");
        free(temp_string);
        if(file_for_open == NULL){
            printf("File named - \"%s\" not found.\n",argv[file_index]);
        }
        else{
            pre_assembler(file_for_open,argv[file_index]);
            fclose(file_for_open);
            temp_string = concat_string(argv[file_index],".am");
            file_for_assembler = fopen(temp_string,"r");
            free(temp_string);
            fseek(file_for_assembler,0,SEEK_SET);
            first_pass_bool = first_pass(file_for_assembler,&label_array,&label_array_index,&data_array,&data_array_index,&IC,&DC);
            if(first_pass_bool){
                fseek(file_for_assembler,0,SEEK_SET);
                second_pass_bool = second_pass(file_for_assembler,label_array,label_array_index,data_array,data_array_index,IC,DC,argv[file_index]);
            }
            if(first_pass_bool && second_pass_bool){
                printf("\"%s.as\" ran successfully!\n",argv[file_index]);
            }
            else {
            printf("\"%s.as\" failed to run.\n",argv[file_index]);
            }            

            fclose(file_for_assembler);
        }
        free(label_array);
        free(data_array);

    }
    return 0;
}
