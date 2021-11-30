#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "logarp.h"

/* The function log_entry will be used by all the porcesses, which is why it has been defined in a 
separate module. It takes as inputs the name of the logfile, the nature of the message (Info, warn, ...),
the name of the process calling it (and at what line), and of course, a message*/

void log_entry(char *filename, char *nature, char *file, int line, char *msg )
{	
	//We want to get the time at the moment of the entry
	time_t reft = time(NULL);
    struct tm *timenow;
    timenow = localtime(&reft);

	// We then format it so that we get the following (ex) 18/11/2021 19:18:23
    char timestp[30];
    strftime(timestp, 30, "%d/%m/%Y %H:%M:%S", timenow);
	
	/* Now we want to access the logfile whose name is filename
	We open it with the option "a", which will append the new content to the existing file,
	or will create the file if no file has the name "filename"*/
	FILE *fptr;
    fptr = fopen(filename, "a"); 

	//We check if the openning was succesful or not
    if (fptr == NULL)
    {
		printf("ERROR: cmd_shell could not open the logfile named %s", filename);
		fflush(stdout);
		exit(1);
	}
   
    /* If no problem occured, we then add a line with the current time, the name of the process
	making the entry, the nature of the message and the content of it. */
	fprintf(fptr, "[%s] %s, File %s(%d) - %s\n", nature, timestp, file, line, msg);
	fflush(stdout);

	// We then close the file
	fclose(fptr);
	
}
