/*
 ============================================================================
 Name        : test_escape_chars.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test for verification of escape/special chars handling
 ============================================================================
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

