#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PAUSE 112
#define RESUME 114
const int commands[2] = {PAUSE, RESUME};

void reset()
{
    char msg[1];
    msg[0] = 'r';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent reset\n");
    fflush(stdout);
    close(fd1);
    return;
}

void set_mode(int want_key)
/* Set the terminal in such a way that what is written as input does not appear */
{
    static struct termios old, new_one;

    if (!want_key)
    { /*If we detected an input from the user, we set the attributes to the previous values, with an immediate update*/
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
        //return;
    }

    else
    { /*if nothing was detected, we change get the old settings to disabled the echo to the terminal as well as the erasure and kill of the inpur*/
        tcgetattr(STDIN_FILENO, &old);
        new_one = old;
        new_one.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_one);
    }
}

void pause_prog()
{
    char msg[1];
    msg[0] = 'p';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent pause\n");
    fflush(stdout);
    close(fd1);
    return;
}

void resume()
{
    char msg[1];
    msg[0] = 't';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent resume\n");
    fflush(stdout);
    close(fd1);
    return;
}

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
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

int get_key()
{
    int c = 0;
    struct timeval tv;
    fd_set fs;
    tv.tv_usec = tv.tv_sec = 0;
    int ret_val; // m, n;
    //char line[80];

    FD_ZERO(&fs);
    FD_SET(STDIN_FILENO, &fs);
    ret_val = select(STDIN_FILENO + 1, &fs, 0, 0, &tv);

    if (ret_val == -1)
        perror("select()");
    else if (ret_val)
    {
        // (FD_ISSET(STDIN_FILENO, &fs)) will be true
        c = getchar();
        set_mode(0);
    }
    return c;
}

int is_command(int pressed_key, const int cmds[6])
{
    /*check if the key pressed belongs to the set of commands*/
    for (int idx = 0; idx < 2; idx++)
    {
        if (pressed_key == cmds[idx])
            return 1;
    }
    return 0;
}

void action(int cmd)
{
    switch (cmd)
    {
    case PAUSE:
        pause_prog();
        int c;
        while (1)
        {
            set_mode(1);
            if (c = get_key())
            {
                if (c == RESUME)
                {
                    action(c);
                    break;
                }
            }
        }
        break;
    case RESUME:
        resume();
        break;
    }
}

int main()
{
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        printf("\ncan't catch RESET\n");
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

    int c;

    printf("initialization complete \n");
    fflush(stdout);

    while (1)
    {
        set_mode(1);
        if (c = get_key())
        {
            if (c == PAUSE)
            {
                action(c);
            }
        }
        get_position(&x, &y, rows, cols, display, fifomot1, fifomot2);
        set_position(x, y, rows, cols, display);
        show_display(rows, cols, display);
    }
}

//create reset buttons
