#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include "../logarp/logarp.h"

/* The watchdog looks at the last modified time of the current logfile,
 and resets the different processes after 1min of inactivity */
void getFileCreationTime(char *path, struct tm *last_time)
{
    //get last modified time of a file
    struct stat attr;
    struct tm *last_modified_time;
    stat(path, &attr);
    //last modified time added
    last_modified_time = localtime(&attr.st_mtim.tv_sec);
    //only change last_time variable if it is different from the last modification of file 
    if (last_modified_time->tm_hour != last_time->tm_hour || last_modified_time->tm_min != last_time->tm_min || last_modified_time->tm_sec != last_time->tm_sec)
    {
        last_time->tm_hour = last_modified_time->tm_hour;
        last_time->tm_sec = last_modified_time->tm_sec;
        last_time->tm_min = last_modified_time->tm_min;
    }
}

void reset()
{
    //send reset signal to master via pipe
    char msg[1];
    msg[0] = 'u';
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

void kill_prog()
{
    char msg[1];
    msg[0] = 'q';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent kill\n");
    fflush(stdout);
    close(fd1);
    return;
}

void sig_handler(int signo)
{
    if (signo == SIGTSTP || signo == SIGINT){
        kill_prog();
    }
}

int main(int argc, char *argv[])
{
    if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGTSTP\n");
        }
     if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGINT\n");
        }
    //starting log
	log_entry(argv[1], "INFO",  __FILE__, __LINE__, "Execution started");
    time_t rawtime;
    struct tm last_time;
    struct tm *current_time;
    int time_spent = 0;
    while (1)
    {
        //get last file modification 
        getFileCreationTime(argv[1], &last_time);
        time(&rawtime);
        //get current time
        current_time = localtime(&rawtime);
        time_spent = (current_time->tm_hour * 3600 - last_time.tm_hour * 3600) + (current_time->tm_min * 60 - last_time.tm_min * 60) + (current_time->tm_sec - last_time.tm_sec);
        printf("time_spent : %i\n", time_spent);
        //if nothing happened for a minute
        if (time_spent > 60)
        {
            reset();
            //wait a bit to not reset twice in a row
            sleep(30);
        }
    }
}
