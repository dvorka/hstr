/*
 ============================================================================
 Name        : test_utf8.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
*/

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <ncurses.h>
#include <readline/readline.h>
#include <readline/chardefs.h>

void console_echo_czech()
{    
    int c;
    while(1) {
      c = getc(stdin);
      printf("\nKey: '%3d', char: '%c'", c, c);
    }
}

void console_static_wide_czech()
{
  setlocale(LC_ALL, "");

  wchar_t *w=L"Čeština."; // wide
  char multibyte[100]; // multi-byte
  if(iswprint(*w)) {
	  printf("\nString to be printed is UTF8 wide!");
	  int offset=wctomb(multibyte, w);
	  printf("\nStatic (wide) printf: %s", multibyte);
  } else {
	  printf("\nString to be printed is NOT UTF8 wide!");
	  wprintf(L"\nStatic wprintf: %ls", w);
  }
}

void console_static_czech()
{
  setlocale(LC_ALL, "");

  char *s="Čeština ěščřžýáíé.";  
  printf("\nStatic printf: %s", s);
}

void console_check() 
{
  printf("\nEnglish string.");
}

void curses_wide_czech()
{
  char *s="Čeština ěščřžýáíé.";  
  printf("Going to print the following string in Curses: '%s'", s);
  getch();

  WINDOW *stdscr;
  setlocale(LC_ALL, "");

  stdscr=initscr();
  mvwprintw(stdscr,0,0,s);
  getch();
  endwin();
}

int main(int argc, char *argv[])
{
  //console_check();
  //console_static_czech();
  //console_static_wide_czech();
  //console_echo_czech();

  // TODO study print_selection_row()
 
  curses_wide_czech();

  printf("\n\n");
}
