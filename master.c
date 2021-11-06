#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define NUMBER_OF_PROCESSES 5
#define MAX_NAME_SIZE 40

int main()
{
    char processes[NUMBER_OF_PROCESSES][MAX_NAME_SIZE] =
        {"./watchdog",
         "./cmd_shell",
         "./display",
         "./motor1",
         "./motor2"};
    int pid_list[NUMBER_OF_PROCESSES];
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
    }
}