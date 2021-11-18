#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>

#include "logarp.h"

void log_entry(char *filename, char *nature, char *file, int line, char *msg )
{	
	time_t reft = time(NULL);
    struct tm *timenow;
    timenow = localtime(&reft);
    char timestp[30];
    strftime(timestp, 30, "%d/%m/%Y %H:%M:%S", timenow);
	
	
	FILE *fptr;
    fptr = fopen(filename, "a");
    if (fptr == NULL)
    {
		printf("ERROR: cmd_shell could not open the logfile named %s", filename);
		fflush(stdout);
		exit(1);
	}
   
    
	fprintf(fptr, "[%s] Time:%s, File:%s(%d) - %s\n", nature, timestp, file, line, msg);
	fflush(stdout);
	fclose(fptr);
	
}
