#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

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

    x = (int)x;
    y = (int)y;
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
            else if (display[i][j] == 'x'){
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

    char format_string_mot1[80] = "%f";
    char format_string_mot2[80] = "%f";

    float x_get = 0.;
    float y_get = 0.;

    sscanf(linemot1, format_string_mot1, &x_get);
    sscanf(linemot2, format_string_mot2, &y_get);

    *x = x_get;
    *y = y_get;

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

int main()
{
    size_t rows = 0;
    size_t cols = 0;

    rows = 15;
    cols = 15;

    char display[rows][cols];

    create_display(rows, cols, display);
    show_display(rows, cols, display);

    char *fifomot1 = "/tmp/motor1";
    mkfifo(fifomot1, 0666);

    
    char *fifomot2 = "/tmp/motor2";
    mkfifo(fifomot2, 0666);

    double x = cols+1;
    double y = rows+1;

    printf("initialization complete \n");
    fflush(stdout);

    while (1)
    {
        get_position(&x, &y, rows, cols, display, fifomot1, fifomot2);
        set_position(x, y, rows, cols, display);
        show_display(rows, cols, display);
        sleep(1);
    }
}

//create reset buttons
