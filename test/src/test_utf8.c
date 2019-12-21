/*
 test_*.c       HSTR test

 Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <curses.h>
#elif defined(__FreeBSD__)
#include <ncurses.h>
#else
#include <ncursesw/curses.h>
#endif
#include <readline/readline.h>
#include <readline/chardefs.h>

#define BITS_IN_BYTE 8
#define INTEGRAL_TYPE char
void show_bits(INTEGRAL_TYPE x) {
    int i;
    static int intSizeInBits = sizeof(INTEGRAL_TYPE) * BITS_IN_BYTE;
    static char symbol[2] = {'0','1'};
    char * binary = (char *)malloc(intSizeInBits + 1);

    memset(binary, 0, intSizeInBits + 1);

    for (i=0; i< intSizeInBits; i++) {
        binary[intSizeInBits-i-1] = symbol[(x>>i) & 0x01];
    }

    printf("\n%s", binary);
    printf("\n1234567.1234567.1234567.1234567.");
    free(binary);
}

void console_echo_czech()
{
    int c;
    while(1) {
      c = getc(stdin);
      printf("\nKey: '%3d', char: '%c'", c, c);
      show_bits(c);
    }
}

void loop_string() {
//      char *s="a";
      char *s="Ča";
//      char *s="Čeština";
      int i;
      for(i=0; i<10; i++) {
          printf("\n%d",s[i]);
          show_bits(s[i]);
          if(!s[i]) break;
      }
}

void get_string_length() {
      char *s="Čeština";
      wchar_t *w=L"Čeština";

      printf("%s (7): strlen(): %zd, mbstowcs(): %zd, wcslen(): %zd",
              s,
              strlen(s),
              mbstowcs(NULL,s,0),
              wcslen(w));         // OK
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

void print_char_bits(int y, int x, char c) {
    int i;
    static int intSizeInBits = sizeof(char) * 8;
    static char symbol[2] = {'0','1'};
    char * binary = (char *)malloc(intSizeInBits + 1);
    memset(binary, 0, intSizeInBits + 1);
    for (i=0; i< intSizeInBits; i++) {
        binary[intSizeInBits-i-1] = symbol[(c>>i) & 0x01];
    }
    mvprintw(y, x, "%s", binary);
    free(binary);

}

void getch_with_counter_curses() {
    // TODO implement getch with counter; getch result analysis (trip); append analysis; ...

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    use_default_colors();

    char pattern[512];
    int c;

    pattern[0]=0;
    while (1) {
        c = wgetch(stdscr);
        strcat(pattern, (char*)(&c));
        mvprintw(2, 0, "Pattern '%s'", pattern);
        mvprintw(3, 0, "Char    '%d'", c);

        mvprintw(6, 0, "strlen()   '%d'", strlen(pattern));
        mvprintw(7, 0, "mbstowcs() '%d'", mbstowcs(NULL,pattern,0));

        int i;
        int intSizeInBits = sizeof(int) * 8;
        char symbol[2] = {'0','1'};
        char * binary = (char *)malloc(intSizeInBits + 1);
        memset(binary, 0, intSizeInBits + 1);
        for (i=0; i< intSizeInBits; i++) {
            binary[intSizeInBits-i-1] = symbol[(c>>i) & 0x01];
        }
        mvprintw(10, 0, "bits:     %s", binary);
        free(binary);

        mvprintw(11, 0, "high bit: %d %d      ", 1<<7, 1<<7 & c);

        char cc=pattern[0];
        i=0;
        int myStrlen=0;
        char isHighBitSet=0;
        while(cc) {
            print_char_bits(12, 9*i-8, pattern[i++]);
            cc=pattern[i];

            if(1<<7 & pattern[i]) {
                if(isHighBitSet) {
                    isHighBitSet=0;
                    myStrlen++;
                } else {
                    isHighBitSet=1;
                }
            } else {
                myStrlen++;
            }
        }

        mvprintw(14, 0, "mystrlen(): %d   ", myStrlen);
    }

    clear();
    refresh();
    doupdate();
    endwin();
}

void done() {
      printf("\n\n");
}

int main(int argc, char *argv[])
{
  //console_check();
  //console_static_czech();
  //console_static_wide_czech();
  //console_echo_czech();
  //curses_wide_czech();
  //get_string_length();
  //loop_string();

    getch_with_counter_curses();
    done();
}
