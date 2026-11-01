/*
 hstr_selection.c       selection cursor navigation

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

#include "include/hstr_selection.h"
#include "include/hstr_curses.h"
#include "include/hstr_utils.h"

// appends line to the selection unless it is full or (if noDuplicates) already present,
// returns true if the line was added
bool hstr_selection_add(char** selection, unsigned capacity, unsigned* selectionSize, char* line, bool noDuplicates)
{
    if(!selection || !selectionSize || !line || *selectionSize>=capacity) {
        return false;
    }
    if(noDuplicates) {
        unsigned i;
        for(i=0; i<*selectionSize; i++) {
            if(selection[i] && !strcmp(selection[i], line)) {
                return false;
            }
        }
    }
    selection[*selectionSize]=line;
    (*selectionSize)++;
    return true;
}

// returns rows by which a navigation key moves the selection cursor (0 for other keys)
int hstr_selection_cursor_step(int key)
{
    switch(key) {
    case KEY_UP:
    case K_CTRL_K:
    case K_CTRL_P:
        return -1;
    case KEY_DOWN:
    case K_CTRL_J:
    case K_CTRL_N:
        return 1;
    case KEY_PPAGE:
        return -PG_JUMP_SIZE;
    case KEY_NPAGE:
        return PG_JUMP_SIZE;
    default:
        return 0;
    }
}

int hstr_selection_index(int cursor, bool promptBottom, int promptItems, int selectionSize)
{
    if(cursor==SELECTION_CURSOR_IN_PROMPT) {
        return -1;
    }
    // with the prompt at the bottom the first item is on the last row
    int index=promptBottom ? promptItems-cursor-1 : cursor;
    // the selection array may be larger than selectionSize (stale items)
    return (index>=0 && index<selectionSize) ? index : -1;
}

int hstr_selection_cursor_move(int cursor, int step, bool promptBottom, int promptItems, int selectionSize)
{
    // rows occupied by the selection items - only these may be selected
    int rows=MIN(selectionSize, promptItems);
    if(rows<=0) {
        return SELECTION_CURSOR_IN_PROMPT;
    }
    int first=promptBottom ? promptItems-rows : 0;
    int last=first+rows-1;

    if(cursor==SELECTION_CURSOR_IN_PROMPT) {
        // moving up from the prompt enters at the last row, any other move at the first row
        return step==-1 ? last : first;
    }

    int moved=cursor+step;
    if(step==-1 || step==1) {
        // line moves wrap around
        if(moved<first) return last;
        if(moved>last) return first;
        return moved;
    }
    // page moves stop at the edges
    return moved<first ? first : (moved>last ? last : moved);
}
