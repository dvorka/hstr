/*
 ============================================================================
 Name        : hstr_utils.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Utilities
 ============================================================================
*/

#include "include/hstr_utils.h"

#define DEFAULT_COMMAND "pwd"
#define HOSTNAME_BUFFER 100
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

char *get_hostname()
{
    char *b=malloc(HOSTNAME_BUFFER);
	if(access( PROC_HOSTNAME, F_OK ) != -1) {
	    FILE *f = fopen(PROC_HOSTNAME, "r");
	    b=fgets(b, HOSTNAME_BUFFER, f);
	    fclose(f);
	    if(b) {
	    	b[strlen(b)-1]=0;
	    	return b;
	    }
	}
    strcpy(b,"localhost");
    return b;
}

