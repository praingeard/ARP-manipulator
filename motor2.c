#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main()
{

    double y = 0;

    char *fifomot2 = "/tmp/motor";
    mkfifo(fifomot2, 0666);

    while (1)
    {
        read_input(&y);
        set_position(&y);
        write_position(y, fifomot2);
        sleep(1);
    }
}

void read_input(double *y)
{
    char recieved[1] = "";
    int fd1;
    int res;
    char *myfifo = "/tmp/z_motor";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_RDONLY);
    res = read(fd1, recieved, 1);
    printf("%s\n", recieved);
    fflush(stdout);

    close(fd1);
    sleep(1);
    return;
}

void write_position(double y, char *fifomot2)
{
    int fd1;
    char input_string[80];
    char format_string[80] = "%c,%f";
    sprintf(input_string, format_string, 'y',y);
    printf("before writing value %f\n", y);
    fflush(stdout);
    fd1 = open(fifomot2, O_WRONLY);
    printf("writing value %f\n", y);
    fflush(stdout);
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}

void set_position(double *y)
{
    *y = *y + 1;
}