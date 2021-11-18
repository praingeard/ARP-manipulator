#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>
#include "logarp.h"

void reset()
{
    char msg[1];
    msg[0] = 'r';
    int fd1,res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent reset\n");
    fflush(stdout);
    close(fd1);
    return;
}

void sig_handler(int signo)
{
    if (signo == SIGINT){
        reset();
    }
}

void create_display(size_t rows, size_t cols, char (*display)[cols])
{
    size_t i, j;

    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            if (j == cols / 2)
            {
                display[i][j] = '|';
            }
            else if (i == 1)
            {
                display[i][j] = '_';
            }
            else
            {
                display[i][j] = '.';
            }
}

void set_position(double x, double y, size_t rows, size_t cols, char (*display)[cols])
{
    size_t i, j;

    x = (int)round(x);
    y = (int)round(y);
    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            if (i == x)
            {
                if (j == y)
                {
                    display[i][j] = 'x';
                }
                else
                {
                    display[i][j] = '_';
                }
            }
            else if (j == cols / 2)
            {
                display[i][j] = '|';
            }
            else if (display[i][j] == '_')
            {
                display[i][j] = '.';
            }
            else if (display[i][j] == 'x')
            {
                display[i][j] = '.';
            }
}

void get_position(double *x, double *y, size_t rows, size_t cols, char (*display)[cols], char *fifomot1, char *fifomot2)
{
    char linemot1[80] = "";
    char linemot2[80] = "";

    int fd1;
    int fd2;
    int resmot1;
    int resmot2;

        fd1 = open(fifomot1, O_RDONLY);
        fd2 = open(fifomot2, O_RDONLY);
        resmot1 = read(fd1, linemot1, 80);
        resmot2 = read(fd2, linemot2, 80);

        char format_string_mot1[80] = "%c,%f";
        char format_string_mot2[80] = "%c,%f";


        float value_get1 = 0.;
        char value_char1;
        float value_get2 = 0.;
        char value_char2;

        sscanf(linemot1, format_string_mot1, &value_char1, &value_get1);
        sscanf(linemot2, format_string_mot2, &value_char2, &value_get2);
        *x = value_get1;
        *y = value_get2;

        printf("x is %f \n", *x);
        fflush(stdout);
        printf("y is %f \n", *y);
        fflush(stdout);

        close(fd1);
        close(fd2);
}

void show_display(size_t rows, size_t cols, char (*display)[cols])
{

    size_t i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            printf("%c ", display[i][j]);
            fflush(stdout);
            if (j == cols - 1)
            {
                printf("\n");
            }
        }
    }
    printf("\n");
    return;
}

int main(int argc, char *argv[])
{
	log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Execution started");
    if (signal(SIGINT, sig_handler) == SIG_ERR){
            printf("\ncan't catch SIGINT\n");
        }
    
    size_t rows = 0;
    size_t cols = 0;

    rows = 15;
    cols = 15;

    char display[rows][cols];

    create_display(rows, cols, display);
    show_display(rows, cols, display);

    char *fifomot1 = "/tmp/motor";
    mkfifo(fifomot1, 0666);
     char *fifomot2 = "/tmp/motor2";
    mkfifo(fifomot2, 0666);


    double x = cols + 1;
    double y = rows + 1;

    printf("initialization complete \n");
    fflush(stdout);

    while (1)
    {
        get_position(&x, &y, rows, cols, display, fifomot1, fifomot2);
        set_position(x, y, rows, cols, display);
        show_display(rows, cols, display);
    }
}

//create reset buttons
