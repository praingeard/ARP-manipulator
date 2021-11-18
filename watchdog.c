#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "logarp.h"

void getFileCreationTime(char *path, struct tm *last_time)
{
    struct stat attr;
    struct tm *last_modified_time;
    stat(path, &attr);
    last_modified_time = localtime(&attr.st_mtim);
    if (last_modified_time->tm_hour != last_time->tm_hour || last_modified_time->tm_min != last_time->tm_min || last_modified_time->tm_sec != last_time->tm_sec)
    {
        last_time->tm_hour = last_modified_time->tm_hour;
        last_time->tm_sec = last_modified_time->tm_sec;
        last_time->tm_min = last_modified_time->tm_min;
    }
}

void reset()
{
    char msg[1];
    msg[0] = 'r';
    int fd1,res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent reset\n");
    fflush(stdout);
    close(fd1);
    return;
}

int main(int argc, char *argv[])
{
	log_entry(argv[1], "INFO",  __FILE__, __LINE__, "Execution started");
    time_t rawtime;
    struct tm last_time;
    struct tm *current_time;
    int time_spent = 0;
    while (1)
    {
        char *path = "log.txt";
        getFileCreationTime(path, &last_time);
        time(&rawtime);
        current_time = localtime(&rawtime);
        time_spent = (current_time->tm_hour * 3600 - last_time.tm_hour * 3600) + (current_time->tm_min * 60 - last_time.tm_min * 60) + (current_time->tm_sec - last_time.tm_sec);
        printf("time_spent : %i\n", time_spent);
        if (time_spent > 3600)
        {
            reset();
            sleep(30);
        }
    }
}
