#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include "../logarp/logarp.h"

char logname[40] = "log.txt";
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
    msg[0] = 'r';
    int fd1,res;
    char *myfifo = "/tmp/reset";
    if (mkfifo(myfifo, 0666) == -1){
        log_entry(logname, "INFO",  __FILE__, __LINE__, "reset tube already exists");
    }

    if (fd1 = open(myfifo, O_WRONLY) == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "reset tube could not be opened");
		exit(EXIT_FAILURE);
    };
    if (res = write(fd1, msg, 2) == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "reset tube could not be written on");
		exit(EXIT_FAILURE);
    };

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

    if (mkfifo(myfifo, 0666) == -1){
        log_entry(logname, "INFO",  __FILE__, __LINE__, "kill tube already exists");
    }
    
    if (fd1 = open(myfifo, O_WRONLY) == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "kill tube could not be opened");
		exit(EXIT_FAILURE);
    };
    if (res = write(fd1, msg, 2) == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "kill tube could not be written on");
		exit(EXIT_FAILURE);
    };

    printf(" sent kill\n");
    fflush(stdout);
    close(fd1);
    return;
}

void sig_handler(int signo)
{
    if (signo == SIGTSTP || signo == SIGINT){
        log_entry(logname, "INFO",  __FILE__, __LINE__, "Program kill by user");
        kill_prog();
    }
}

int main(int argc, char *argv[])
{
    strncpy(logname, argv[1], 39);
    logname[39] = 0;
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
    sleep(2);
    while (1)
    {
        //get last file modification 
        getFileCreationTime(argv[1], &last_time);
        time(&rawtime);
        //get current time
        current_time = localtime(&rawtime);
        time_spent = (current_time->tm_hour * 3600 - last_time.tm_hour * 3600) + (current_time->tm_min * 60 - last_time.tm_min * 60) + (current_time->tm_sec - last_time.tm_sec);
        //if nothing happened for a minute
        if (time_spent > 60)
        {
            log_entry(argv[1], "INFO",  __FILE__, __LINE__, "Sent RESET");
            reset();
            //wait a bit to not reset twice in a row
            sleep(30);
        }
    }
}
