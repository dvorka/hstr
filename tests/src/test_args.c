/*
 ============================================================================
 Name        : test_args.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
*/

#include <string.h>
#include <stdio.h>

#define LINELNG 500

int main(int argc, char *argv[])
{
	printf("%d",strchr("a\nb",10));

	if(argc>0) {
		int i;
		char line[LINELNG];
		line[0]=0;
		for(i=0; i<argc; i++) {
			if((strlen(line)+strlen(argv[i])*2)>LINELNG) break;
			printf("%d %s\n", i, argv[i]);
			if(strstr(argv[i], " ")) {
				strcat(line, "\"");
			}
			strcat(line, argv[i]);
			if(strstr(argv[i], " ")) {
				strcat(line, "\"");
			}
			strcat(line, " ");
		}

		printf("#%s#", line);
	}
}
