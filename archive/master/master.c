#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../logarp/logarp.h"

#define NUMBER_OF_PROCESSES_KONSOLE 5
#define NUMBER_OF_PROCESSES_BACKGROUND 5
#define MAX_NAME_SIZE 40

void reset(pid_t pid_mot1, pid_t pid_mot2, pid_t pid_display)
{
    //send reset signals to every process

    kill(pid_display, SIGUSR1);
    kill(pid_mot1, SIGINT);
    kill(pid_mot2, SIGINT);
}

void pause_prog(pid_t pid_mot1, pid_t pid_mot2)
{
    //send pause signals to motors

    kill(pid_mot1, SIGUSR1);
    kill(pid_mot2, SIGUSR2);
}

void resume(pid_t pid_mot1, pid_t pid_mot2)
{
    //send resume signals to motors

    kill(pid_mot1, SIGUSR2);
    kill(pid_mot2, SIGUSR2);
}

int main()
{
    //logfile initializing
    time_t reft = time(NULL);
    struct tm *timenow;
    timenow = localtime(&reft);
    char logname[40];
    strftime(logname, 40, "../logs/log_%d-%m-%Y_%H:%M:%S.txt", timenow);

    const char *logFileName = logname;
    printf("opening log file : %s \n", logFileName);
    fflush(stdout);

    //get processes names to launch in console
    char processes_konsole[NUMBER_OF_PROCESSES_KONSOLE][MAX_NAME_SIZE] =
        {"../cmd_shell/cmd_shell",
         "../display/display"};

    //processes to launch in background
    char processes_background[NUMBER_OF_PROCESSES_BACKGROUND][MAX_NAME_SIZE] =
        {"../watchdog/watchdog",
         "../motor1/motor1",
         "../motor2/motor2"};
    char actualpath [100];
    for (int i = 0; i < NUMBER_OF_PROCESSES_KONSOLE; i++)
    {
        //initialize children
        pid_t child = fork();
        if (child == 0)
        {
            //exec konsole command
            execl("/bin/konsole", "/bin/konsole", "-e", realpath(processes_konsole[i], actualpath), logname, (char *)0);
        }
    }
    for (int i = 0; i < NUMBER_OF_PROCESSES_BACKGROUND; i++)
    {
        //initialize children
        pid_t child = fork();
        if (child == 0)
        {
            //exec in the background
            char args[3];
            args[0] =  realpath(processes_background[i], actualpath);
            args[1] = logname;
            args[2] = NULL;
            execv(args[0], args);
        }
    }
    while (1)
    {
        //master code
        char msg[1];
        int fd1, res;
        //wait for reset or pause
        char *myfifo = "/tmp/reset";
        mkfifo(myfifo, 0666);
        fd1 = open(myfifo, O_RDONLY);
        res = read(fd1, msg, 2);
        printf("got reset val\n");
        fflush(stdout);
        close(fd1);

        //get pid of the motors and the display
        char line[80];
        FILE *cmd = popen("pidof motor1", "r");
        fgets(line, 80, cmd);
        pid_t pid = strtoul(line, NULL, 10);
        printf("pid_process %i\n", pid);
        fflush(stdout);
        pclose(cmd);

        char line2[80];
        FILE *cmd2 = popen("pidof motor2", "r");
        fgets(line2, 80, cmd2);
        pid_t pid2 = strtoul(line2, NULL, 10);
        printf("pid_process %i\n", pid2);
        fflush(stdout);
        pclose(cmd2);

        char line3[80];
        FILE *cmd3 = popen("pidof display", "r");
        fgets(line3, 80, cmd3);
        pid_t pid_display = strtoul(line3, NULL, 10);
        printf("pid_process %i\n", pid_display);
        fflush(stdout);
        pclose(cmd3);

        //r is reset, p is pause, t is resume
        if (msg[0] == 'r')
        {
            reset(pid, pid2, pid_display);
        }
        if (msg[0] == 'p')
        {
            pause_prog(pid, pid2);
        }
        if (msg[0] == 't')
        {
            resume(pid, pid2);
        }
    }
}
