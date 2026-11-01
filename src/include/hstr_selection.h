/*
 hstr_selection.h       header file for HSTR selection cursor navigation

 Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifndef HSTR_SELECTION_H
#define HSTR_SELECTION_H

#include <stdbool.h>

// selection cursor is the screen row of the selected item (relative to the first
// selection row) or SELECTION_CURSOR_IN_PROMPT if no item is selected
#define SELECTION_CURSOR_IN_PROMPT -1

// rows by which page up/down moves the selection cursor
#define PG_JUMP_SIZE 10

// Ctrl keys which move the selection cursor (in addition to arrow keys)
#define K_CTRL_J 10
#define K_CTRL_K 11
#define K_CTRL_N 14
#define K_CTRL_P 16

bool hstr_selection_add(
   char** selection,
   unsigned capacity,
   unsigned* selectionSize,
   char* line,
   bool noDuplicates
);
int hstr_selection_cursor_step(int key);
int hstr_selection_index(
   int cursor,
   bool promptBottom,
   int promptItems,
   int selectionSize
);
int hstr_selection_cursor_move(
   int cursor,
   int step,
   bool promptBottom,
   int promptItems,
   int selectionSize
);

#endif
