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

#define NUMBER_OF_PROCESSES_KONSOLE 2
#define NUMBER_OF_PROCESSES_BACKGROUND 3
#define MAX_NAME_SIZE 40

pid_t getprogrampid(char *name){
    char line[80];
    char command[] = "pidof ";
    strcat(command, name);
    FILE *cmd = popen(command, "r");
        fgets(line, 80, cmd);
        pid_t pid = strtoul(line, NULL, 10);
        printf("pid_process %i\n", pid);
        fflush(stdout);
        pclose(cmd);
    return pid;
}

void reset()
{
    //send reset signals to every process
    pid_t pid_display = getprogrampid("display");
    pid_t pid_mot1 = getprogrampid("motor1");
    pid_t pid_mot2 = getprogrampid("motor2");
    kill(pid_display, SIGUSR1);
    kill(pid_mot1, SIGINT);
    kill(pid_mot2, SIGINT);
}

void quit_all(){
    pid_t pid_display = getprogrampid("display");
    pid_t pid_mot1 = getprogrampid("motor1");
    pid_t pid_mot2 = getprogrampid("motor2");
    pid_t pid_watchdog = getprogrampid("watchdog");
    pid_t pid_cmdshell = getprogrampid("cmd_shell");
    pid_t pid_konsole;
    for (int i = 0; i < NUMBER_OF_PROCESSES_KONSOLE; i++){
        pid_konsole = getprogrampid("konsole");
        kill(pid_konsole, SIGTERM);
    }
    kill(pid_display, SIGTERM);
    kill(pid_mot1, SIGTERM);
    kill(pid_mot2, SIGTERM);
    kill(pid_watchdog, SIGTERM);
    kill(pid_cmdshell, SIGTERM);
    exit(EXIT_SUCCESS);
}

void pause_prog()
{
    //send pause signals to motors
    pid_t pid_mot1 = getprogrampid("motor1");
    pid_t pid_mot2 = getprogrampid("motor2");

    kill(pid_mot1, SIGUSR1);
    kill(pid_mot2, SIGUSR2);
}

void stop_prog()
{
    //send pause signals to motors
    pid_t pid_mot1 = getprogrampid("motor1");
    pid_t pid_mot2 = getprogrampid("motor2");
    kill(pid_mot1, SIGIO);
    kill(pid_mot2, SIGIO);
}

void resume()
{
    //send resume signals to motors
    pid_t pid_mot1 = getprogrampid("motor1");
    pid_t pid_mot2 = getprogrampid("motor2");

    kill(pid_mot1, SIGUSR2);
    kill(pid_mot2, SIGUSR2);
}

void sig_handler(int signo)
{
    if (signo == SIGTSTP || signo == SIGINT){
        quit_all();
    }
}


int main()
{
    if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGTERM\n");
        }
     if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGINT\n");
        }
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
        { "../watchdog/watchdog",
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
            char* args[3];
            args[0] =  realpath(processes_background[i], actualpath);
            args[1] = logname;
            args[2] = NULL;
            execvp(args[0], args);
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

        //r is reset, p is pause, t is resume, q is quit, s is stop
        if (msg[0] == 'r')
        {
            reset();
        }
        if (msg[0] == 'p')
        {
            pause_prog();
        }
        if (msg[0] == 't')
        {
            resume();
        }
        if (msg[0] == 'q')
        {
            quit_all();
        }
         if (msg[0] == 's')
        {
            stop_prog();
        }
    }
}
