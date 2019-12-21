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
#include <math.h>

void testLog() {
    const int HISTORY_SIZE=2000;
    int i;
    for(i=0; i<HISTORY_SIZE; i++) {
        printf("\n%d # l: %f # l10: %f # l2: %f", i, log(i), log10(i), log2(i));
    }
}

#define MAX_CHARACTER_CODE 10000
static char line[MAX_CHARACTER_CODE];

void testGenerateHugeHistoryFileWithDifferentLines()
{
    FILE *file = fopen(".bash_history_huge","a");
    fseek(file,0, SEEK_END);

    line[0]=0;
    int i;
    for(i=0; i<MAX_CHARACTER_CODE; i++) {
        sprintf(line,"%s%c",line,i);
        fprintf(file,"%s\n",line);
    }
    fclose(file);
}


void testGenerateHugeHistoryFileWithSameLines()
{
    FILE *file = fopen(".bash_history_same","a");
    fseek(file,0, SEEK_END);

    int i;
    for(i=0; i<100000; i++) {
        fprintf(file,"find . | while read X; do echo $X; cat $X | grep -i ctrl; done | less\n",line);
        fprintf(file,
                "git commit -a -m \"Code review and stabilization.\" && git push origin master#"
                "git commit -a -m \"Code review and stabilization.\" && git push origin master#"
                "git commit -a -m \"Code review and stabilization.\" && git push origin master#"
                "git commit -a -m \"Code review and stabilization.\" && git push origin master#"
                "git commit -a -m \"Code review and stabilization.\" && git push origin master#"
                "git commit -a -m \"Code review and stabilization.\" && git push origin master#"
                "git commit -a -m \"Code review and stabilization.\" && git push origin master\n",
                line);
    }
    fclose(file);
}


int main(int argc, char *argv[])
{
    testGenerateHugeHistoryFileWithSameLines();
}
