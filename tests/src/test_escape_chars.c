/*
 test_escape_chars.c       A test for verification of escape/special chars handling

 Copyright (C) 2014  Martin Dvorak <martin.dvorak@mindforger.com>

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

void generate_bash_history_with_special_chars() {
    /*
     // crash
     %s

     // substituation > should not happen:
     date -d yesterday +%A
     date -d yesterday +0X1.4C41E000008P-895

     // crash
     mocp -Q "<%state> '%file'\n"
     */
}

void main() {
    generate_bash_history_with_special_chars();
}

