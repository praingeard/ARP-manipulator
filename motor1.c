#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main()
{

    double x = 0;

    char *fifomot1 = "/tmp/motor1";
    mkfifo(fifomot1, 0666);

    while (1)
    {
        read_input(&x);
        set_position(&x);
        write_position(x, fifomot1);
        sleep(1);
    }
}

void read_input(double *x)
{
    char recieved[1] = "";
    int fd1;
    int res;
    char *myfifo = "/tmp/x_motor";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_RDONLY);
    res = read(fd1, recieved, 1);
    printf("%s\n", recieved);
    fflush(stdout);

    close(fd1);
    sleep(1);

    return;
}

void write_position(double x, char *fifomot1)
{
    int fd1;
    char input_string[80];
    char format_string[80] = "%f";
    sprintf(input_string, format_string, x);
    printf("before writing value %f\n", x);
    fflush(stdout);
    fd1 = open(fifomot1, O_WRONLY);
    printf("writing value %f\n", x);
    fflush(stdout);
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}

void set_position(double *x)
{
    *x = *x + 1;
    return;
}