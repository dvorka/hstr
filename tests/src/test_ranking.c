/* Test
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

void testGenerateCrazyHistoryFile()
{
	FILE *file = fopen(".bash_history_crazy","a");
	fseek(file,0, SEEK_END);

	line[0]=0;
	int i;
	for(i=0; i<MAX_CHARACTER_CODE; i++) {
		sprintf(line,"%s%c",line,i);
		fprintf(file,"%s\n",line);
	}

	fclose(file);
}

int main(int argc, char *argv[])
{
	testGenerateCrazyHistoryFile();
}

