#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    return;
}

int main()
{
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
        if (time_spent > 60)
        {
            reset();
        }
    }
}