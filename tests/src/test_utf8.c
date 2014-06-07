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
#include <readline/readline.h>
#include <readline/chardefs.h>

void echo_czech()
{    
    int c;
    while(1) {
      c = getc(stdin);
      printf("\nKey: '%3d', char: '%c'", c, c);
    }
}

void static_wide_czech()
{
  setlocale(LC_ALL, "");

  wchar_t *w=L"Čeština.";
  wprintf(L"\nStatic wprintf: %ls", w);
}

void static_czech()
{
  setlocale(LC_ALL, "");

  char *s="Čeština ěščřžýáíé.";  
  printf("\nStatic printf: %s", s);
}

void check() 
{
  printf("\nEnglish string.");
}


int main(int argc, char *argv[])
{
  check();
  static_czech();
  static_wide_czech();
  echo_czech();
  printf("\n\n");
}
