#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>


void read_input(int *step)
{
    char recieved[1] = "";
    int fd1;
    int res;
    char *myfifo = "/tmp/x_motor";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_RDONLY);

    res = read(fd1, recieved, 1);
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
    else
        *step = 0;

    return;
}

void write_position(double y, char *fifomot2)
{
    int fd1;
    char input_string[80];
    char format_string[80] = "%f";
    sprintf(input_string, format_string, y);
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


int main()
{

    double y = 0;
    int step = 0;

    //initialisation of the random generator
    time_t t;
    srand((unsigned)time(&t));
    char *fifomot2 = "/tmp/motor2";
    mkfifo(fifomot2, 0666);

    while (1)
    {
        read_input(&step);
        set_position(&step, &y);
        write_position(y, fifomot2);
        sleep(1);
    }
}