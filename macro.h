/* This file contains macro linked list structure definition. */
#ifndef MACRO_H
#define MACRO_H
#define MAX_MACRO_NAME_LENGTH 32
#define ADD_TO_LINE_LENGTH 2
#define MAX_WORDS_IN_MACRO 6
#include "globals.h"

/* struct that contains the name and the content of the macro */
typedef struct macro {
	char name[MAX_MACRO_NAME_LENGTH];
	char content[(MAX_ROW_SIZE+ADD_TO_LINE_LENGTH)*MAX_WORDS_IN_MACRO];
	/*we are adding to length so if the line is too long we will be able to discover it in the finding error phase*/
}macro;


#endif