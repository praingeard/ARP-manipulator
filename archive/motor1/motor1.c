#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "../logarp/logarp.h"

char *fifomot1 = "/tmp/motor";
double x = 0.0;
int step = 0;

void read_input(int *step)
{
    char recieved[1];
    int fd1;
    int res;
    char *myfifo = "/tmp/z_motor";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_RDONLY);
    res = read(fd1, recieved, 2);
    if (res < 0)
    {
        printf("no value\n");
        fflush(stdout);
    }
    close(fd1);

    printf("%s\n", recieved);
    fflush(stdout);

    if (recieved[0] == 'p')
        *step = 1;
    else if (recieved[0] == 'm')
        *step = -1;
    else if (recieved[0] == 's')
        *step = 0;

    return;
}

void write_position(double x, char *fifomot1)
{
    int fd1;
    char input_string[80];
    char format_string[80] = "%c,%f";
    char value = 'x';
    sprintf(input_string, format_string, value, x);
    fd1 = open(fifomot1, O_WRONLY);
    printf("writing value %f\n", x);
    fflush(stdout);
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}

void set_position(int *step, double *x)
{
    // random error between 0 and 1
    double err = (double)rand() / (double)RAND_MAX;
    // we modifiy the error so that it is between -0.1 and 0.1
    err = 0.2 * err - 0.1;
    *x = *x + *step * (1 + err);

    if (*x < 0)
        *x = 0;

    if (*x > 14)
        *x = 14;

    return;
}

void sig_handler(int signo)
{
    printf("received SIGNAL\n");
    fflush(stdout);
    if (signo == SIGINT)
    {
        printf("received RESET\n");
        fflush(stdout);
        step = -1;
        while (1)
        {
            if (x < 0.1)
            {
                break;
            }
            set_position(&step, &x);
            write_position(x, fifomot1);
            sleep(1);
        }
        char *myfifo = "/tmp/resetmot1";
        mkfifo(myfifo, 0666);
        int fd1;
        // fd1 = open(myfifo, O_WRONLY);
        // printf("RESET end\n");
        // fflush(stdout);
        // close(fd1);
        step = 0;
    }
    if (signo == SIGUSR1)
    {
        printf("system paused\n");
        fflush(stdout);
        pause();
    }
    if (signo == SIGUSR2){
        printf("system resumed\n");
        fflush(stdout);
    }
}

int main(int argc, char *argv[])
{
	log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Execution started");

    //initialisation of the random generator
    time_t t;
    srand((unsigned)time(&t));
    mkfifo(fifomot1, 0666);

    while (1)
    {
        if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGINT\n");
        }
        if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGUSR1\n");
        }
        if (signal(SIGUSR2, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGUSR2\n");
        }
        read_input(&step);
        set_position(&step, &x);
        write_position(x, fifomot1);
        sleep(1);
    }
}