#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include "../logarp/logarp.h"

char *fifomot2 = "/tmp/motor2";
double y = 0.0;

void sig_handler(int signo)
{
    printf("received SIGNAL\n");
    fflush(stdout);
    if (signo == SIGINT){
        printf("received RESET\n");
        fflush(stdout);
        int step = -1;
        mkfifo(fifomot2, 0666);
        char *fifomot1 = "/tmp/motor";
        mkfifo(fifomot1, 0666);
        int fd1;
        while(1){
            if (y < 0.1){
                 printf("break\n");
                fflush(stdout);
                break;
            }
            printf("running\n");
            fflush(stdout);
            fd1 = open(fifomot1, O_WRONLY);
            close(fd1);
            set_position(&step,&y);
            printf("value is %f\n", y);
            fflush(stdout);
            write_position(y, fifomot2);
            sleep(1);
        }
        char *myfifo = "/tmp/resetmot2";
        mkfifo(myfifo, 0666);
        int fd2;
        fd2 = open(myfifo, O_WRONLY);
        printf("RESET end\n");
        fflush(stdout);
        close(fd2);
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

void read_input(int *step)
{
    char recieved[1];
    int fd1;
    int res;
    char *myfifo = "/tmp/x_motor";
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

void write_position(double y, char *fifomot2)
{
    int fd1;
    char input_string[80];
    char format_string[80] = "%c, %f";
    sprintf(input_string, format_string,'y', y);
    printf("before writing value %f\n", y);
    fflush(stdout);
    fd1 = open(fifomot2, O_WRONLY);
    printf("writing value %f\n", y);
    fflush(stdout);
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}

void set_position(int *step, double *y)
{
    // random error between 0 and 1
    double err = (double)rand() / (double)RAND_MAX;
    // we modifiy the error so that it is between -0.1 and 0.1
    err = 0.2 * err - 0.1;
    *y = *y + *step * (1 + err);

    if (*y < 0)
        *y = 0;
    
    if (*y > 14)
        *y = 14;
    

    return;
}


int main(int argc, char *argv[])
{
	log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Execution started");
    if (signal(SIGINT, sig_handler) == SIG_ERR){
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

    int step = 0;

    //initialisation of the random generator
    time_t t;
    srand((unsigned)time(&t));
    mkfifo(fifomot2, 0666);

    while (1)
    {
        read_input(&step);
        set_position(&step, &y);
        write_position(y, fifomot2);
        sleep(1);
    }
}
