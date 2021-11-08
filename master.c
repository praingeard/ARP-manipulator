#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define NUMBER_OF_PROCESSES 5
#define MAX_NAME_SIZE 40

void reset(pid_t pid_mot1, pid_t pid_mot2)
{
    printf("starting RESET for %i and %i\n", pid_mot1, pid_mot2);
    fflush(stdout);

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
    fd1 = open(myfifo, O_RDONLY);
    fd2 = open(myfifo2, O_RDONLY);
    close(fd1);
    close(fd2);
    printf("RESET got \n");
    fflush(stdout);
}

int main()
{
    char processes[NUMBER_OF_PROCESSES][MAX_NAME_SIZE] =
        {"./watchdog",
         "./cmd_shell",
         "./display",
         "./motor1",
         "./motor2"};
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++)
    {
        pid_t child = fork();
        if (child == 0)
        {
            char args[1] = {processes[i], NULL};
            execl("/bin/konsole", "/bin/konsole", "-e", processes[i], (char *)0);
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

        if (msg[0] == 'r')
        {
            reset(pid, pid2);
        }
    }
}