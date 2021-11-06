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

void get_position(double *x, double *y, size_t rows, size_t cols, char (*display)[cols], char *fifomot1)
{
    char linemot1[80] = "";
    int fd1;
    int resmot1;

    fd1 = open(fifomot1, O_RDONLY);

    resmot1 = read(fd1, linemot1, 80);

    char format_string_mot1[80] = "%c,%f";

    float value_get = 0.;
    char value_char;

    sscanf(linemot1, format_string_mot1, &value_char, &value_get);
    printf("value_got is %c,%f", value_char, value_get);

    if (value_char == 'x'){
        
        *x = value_get;
    }
    else if(value_char == 'y'){
        *y = value_get;
    }

    printf("x is %f \n", *x);
    fflush(stdout);
    printf("y is %f \n", *y);
    fflush(stdout);

    

    close(fd1);
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

    char *fifomot1 = "/tmp/motor";
    mkfifo(fifomot1, 0666);

    double x = cols+1;
    double y = rows+1;

    printf("initialization complete \n");
    fflush(stdout);

    while (1)
    {
        get_position(&x, &y, rows, cols, display, fifomot1);
        set_position(x, y, rows, cols, display);
        show_display(rows, cols, display);
    }
}

//create reset buttons
