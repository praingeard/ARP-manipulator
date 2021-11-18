#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "../logarp/logarp.h"

#define NUMBER_OF_PROCESSES 5
#define MAX_NAME_SIZE 40

void reset(pid_t pid_mot1, pid_t pid_mot2, pid_t pid_display)
{

    printf("starting RESET for %i and %i\n", pid_mot1, pid_mot2);
    fflush(stdout);

    kill(pid_display, SIGUSR1);
    kill(pid_mot1, SIGINT);
    kill(pid_mot2, SIGINT);
    
    char *myfifo = "/tmp/resetmot1";
    mkfifo(myfifo, 0666);
    char *myfifo2 = "/tmp/resetmot2";
    mkfifo(myfifo2, 0666);
    char msg1[1];
    int fd1, res1;
    char msg2[1];
    int fd2, res2;
    // fd1 = open(myfifo, O_RDONLY);
    // fd2 = open(myfifo2, O_RDONLY);
    // close(fd1);
    // close(fd2);
    printf("RESET got \n");
    fflush(stdout);
}

void pause_prog(pid_t pid_mot1, pid_t pid_mot2)
{
    printf("starting PAUSE for %i and %i\n", pid_mot1, pid_mot2);
    fflush(stdout);

    kill(pid_mot1, SIGUSR1);
    kill(pid_mot2, SIGUSR2);
}

void resume(pid_t pid_mot1, pid_t pid_mot2)
{
    printf("starting RESUME for %i and %i\n", pid_mot1, pid_mot2);
    fflush(stdout);

    kill(pid_mot1, SIGUSR2);
    kill(pid_mot2, SIGUSR2);
}

int main()
{
    time_t reft = time(NULL);
    struct tm *timenow;
    timenow = localtime(&reft);
    char logname[40];
    strftime(logname, 40, "../logs/log_%d-%m-%Y_%H:%M:%S.txt", timenow);

    const char *logFileName = logname;
    printf("le fichier à ouvrir est le suivant : %s", logFileName);
    fflush(stdout);

    char processes[NUMBER_OF_PROCESSES][MAX_NAME_SIZE] =
        {"../watchdog/watchdog",
         "../cmd_shell/cmd_shell",
         "../display/display",
         "../motor1/motor1",
         "../motor2/motor2"};
    char actualpath [100];
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++)
    {
        pid_t child = fork();
        if (child == 0)
        {
            execl("/bin/konsole", "/bin/konsole", "-e", realpath(processes[i], actualpath), logname, (char *)0);
        }
    }
    while (1)
    {
        char msg[1];
        int fd1, res;
        char *myfifo = "/tmp/reset";
        mkfifo(myfifo, 0666);
        fd1 = open(myfifo, O_RDONLY);
        res = read(fd1, msg, 2);
        printf("got reset val\n");
        fflush(stdout);
        close(fd1);

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
