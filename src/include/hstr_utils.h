/*
 ============================================================================
 Name        : hstr_utils.h
 Author      : martin.dvorak@mindforger.com
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
#include <unistd.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

char *hstr_strdup(const char *s);
int hstr_strlen(const char *s);
void hstr_chop(char *s);
void tiocsti();
void fill_terminal_input(char* cmd, bool padding);
void reverse_char_pointer_array(char **array, unsigned length);
void get_hostname(int bufferSize, char *buffer);
void toggle_case(char *str, bool lowercase);

#endif
