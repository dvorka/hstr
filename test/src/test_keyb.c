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
