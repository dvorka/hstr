/*
 ============================================================================
 Name        : hstr_utils.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Utilities
 ============================================================================
*/

#include "include/hstr_utils.h"
#include <ctype.h>

#define DEFAULT_COMMAND "pwd"
#define PROC_HOSTNAME "/proc/sys/kernel/hostname"

void tiocsti()
{
	char buf[] = DEFAULT_COMMAND;
	int i;
	for (i = 0; i < sizeof buf - 1; i++) {
		ioctl(0, TIOCSTI, &buf[i]);
	}
}

void fill_terminal_input(char *cmd, bool padding)
{
	if(cmd && strlen(cmd)>0) {
		size_t size = strlen(cmd);
		unsigned i;
		char *c;
		for (i = 0; i < size; i++) {
			// terminal I/O control, simulate terminal input
			c=(cmd+i);
			ioctl(0, TIOCSTI, c);
		}
		// echo, but don't flush to terminal
		if(padding) printf("\n");
	}
}

void reverse_char_pointer_array(char **array, unsigned length)
{
	char *temp;
	unsigned i;
    for (i=0; i<length/2; i++) {
        temp = array[i];
        array[i] = array[length-i-1];
        array[length-i-1] = temp;
    }
}

void get_hostname(int bufferSize, char *buffer)
{
    char *b=buffer;
	if(access(PROC_HOSTNAME, F_OK) != -1) {
	    FILE *file = fopen(PROC_HOSTNAME, "r");
	    b=fgets(buffer, bufferSize, file);
	    fclose(file);
	    if(b) {
	    	b[strlen(b)-1]=0;
	    	return;
	    }
	}
    strcpy(buffer,"localhost");
}

void toggle_case(char *str, bool lowercase) {
	if(str && strlen(str)>0) {
		int i;
		for(i = 0; str[i]; i++){
			if(lowercase) {
				str[i] = tolower(str[i]);
			} else {
				str[i] = toupper(str[i]);
			}
		}
	}
}
