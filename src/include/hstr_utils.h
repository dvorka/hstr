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
#include <stdbool.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void tiocsti();
void fill_terminal_input(char* cmd, bool padding);
void reverse_char_pointer_array(char **array, unsigned length);

#endif
