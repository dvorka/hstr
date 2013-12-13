#include "include/hstr_utils.h"

#define DEFAULT_COMMAND "pwd"

void tiocsti() {
	char buf[] = DEFAULT_COMMAND;
	int i;
	for (i = 0; i < sizeof buf - 1; i++) {
		ioctl(0, TIOCSTI, &buf[i]);
	}
}

void fill_terminal_input(char *cmd){
	size_t size = strlen(cmd);
	int i;
	char *c;
	for (i = 0; i < size; i++) {
		// terminal I/O control, simulate terminal input
		c=(cmd+i);
		ioctl(0, TIOCSTI, c);
	}
	printf("\n");
}

void reverse_char_pointer_array(char **array, int length) {
	int i;
	char *temp;
    for (i=0; i<length/2; i++) {
        temp = array[i];
        array[i] = array[length-i-1];
        array[length-i-1] = temp;
    }
}



