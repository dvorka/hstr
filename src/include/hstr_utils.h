/*
 ============================================================================
 Name        : hstr_utils.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Utilities
 ============================================================================
*/

#ifndef _HSTR_UTILS_H
#define _HSTR_UTILS_H

#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void tiocsti();
void fill_terminal_input(char* cmd);
void reverse_char_pointer_array(char **array, unsigned length);

#endif
