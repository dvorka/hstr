/*
 ============================================================================
 Name        : test_keyb.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
*/

#include <stdio.h>
#include <readline/readline.h>
#include <readline/chardefs.h>

// $ xev
//   ... get keycodes

void echo_printable_characters()
{
	int c;
	for(c=0; c<255; c++) {
		printf("Key number: '%3d' / Char: '%c' Meta: \n", c, c, c&meta_character_bit);
	}
}

void echo_keyb_characters() 
{
	int c;
	while(1) {
		c = getc(stdin);
		printf("Key number: '%3d' / Char: '%c' Meta: %d Ctrl: %d Ctrl mask: %d\n", c, c, META_CHAR(c), CTRL_CHAR(c), c&control_character_mask);
	}
}

int main(int argc, char *argv[])
{
	echo_keyb_characters();
}
