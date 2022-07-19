#ifndef GLOBALS_H
#define GLOBALS_H
#define MAX_COMMAND_NAME_SIZE 5
#define MAX_REGISTER_NAME 4
#define AMOUNT_OF_REGISTERS 17
#define AMOUNT_OF_COMMANDS 17
#define MAX_LABEL_LENGTH 32
#define MAX_ROW_SIZE 81
#define TRUE 1
#define FALSE 0
#define BINARY_CODE_LENGTH 20
#define STARTING_IC 100
#define DYNAMIC_ARRAY_INITIAL_SIZE 10
#define FIRST_PASS_COMMAND_DELIMITER " \t,\n"
#define WHITE_CHAR " \t"
#define WORD_SIZE 20
#define INT_BITS_SIZE 16

/*
We know that you said we need to implement the structs in the .c file but when we tried to implement it that way
our entire program crashed and nothing worked. After a few days of trying to debug it we decided to go back
to implementing it that way. We are sorry and hope you'll understand.
Thank you roy <3
*/


/* struct that represents the labels in the program. saves value,base,offset,symbol and their attributes. */
typedef struct label{
    char symbol[MAX_LABEL_LENGTH];
    int value;
    int base;
    int offset;
    int is_data;
    int is_code;
    int is_entry;
    int is_external;
}label;

/* struct that represents the data commands in the program. */
typedef struct data {
    int DC;
    int is_data;
    int is_string;
    int *num_list;
    int num_list_size;
}data;

/* struct that represents the corresponding opcode and funct of a command. */
typedef struct command_opcode_funct {
    char name[MAX_COMMAND_NAME_SIZE];
    int funct;
    int opcode;
    int num_operand;
}command_opcode_funct;

/* struct that represents the registers in the program. example: name = r12 , register_number = 12 */
typedef struct registers {
    char name[MAX_REGISTER_NAME];
    int register_number;
}registers;

extern const registers registers_list[];
extern const command_opcode_funct command_list[];

#endif