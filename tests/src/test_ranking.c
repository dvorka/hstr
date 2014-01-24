/* Test
 */

#include <stdio.h>
#include <math.h>

void testLog() {
	const int HISTORY_SIZE=2000;
	int i;
	for(i=0; i<HISTORY_SIZE; i++) {
		printf("\n%d # l: %f # l10: %f # l2: %f",i,log(i), log10(i), log2(i));
	}
}

int main(int argc, char *argv[])
{
	testLog();
}

