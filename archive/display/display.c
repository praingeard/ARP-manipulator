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
#include "../logarp/logarp.h"

//get keyboard keys 
#define STOP 115
#define PAUSE 112
#define RESUME 114
const int commands[3] = {PAUSE, RESUME, STOP};
char logname[40] = "log.txt";

int is_paused = 0;
int is_stopped = 0;

void reset()
{
    //send reset value to master
    char msg[1];
    msg[0] = 'r';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    if (fd1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "reset tube could not be opened");
		exit(EXIT_FAILURE);
    }
    res = write(fd1, msg, 2);
    if (res == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "reset tube could not be written on");
		exit(EXIT_FAILURE);
    }
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

void kill_prog()
{
    char msg[1];
    msg[0] = 'q';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    if (fd1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "kill tube could not be opened");
		exit(EXIT_FAILURE);
    }
    res = write(fd1, msg, 2);
    if (res == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "kill tube could not be written on");
		exit(EXIT_FAILURE);
    }
    printf(" sent kill\n");
    fflush(stdout);
    close(fd1);
    return;
}

void pause_prog()
{
    //send pause value to master
    is_paused = 1;
    char msg[1];
    msg[0] = 'p';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    if (fd1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "pause tube could not be opened");
		exit(EXIT_FAILURE);
    }
    res = write(fd1, msg, 2);
    if (res == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "pause tube could not be written on");
		exit(EXIT_FAILURE);
    }
    printf(" sent pause\n");
    fflush(stdout);
    close(fd1);
    return;
}

void stop_prog()
{
    //send pause value to master
    is_stopped = 1;
    char msg[1];
    msg[0] = 's';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    if (fd1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "reset tube could not be opened");
		exit(EXIT_FAILURE);
    }
    res = write(fd1, msg, 2);
    if (res == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "reset tube could not be written on");
		exit(EXIT_FAILURE);
    }
    fflush(stdout);
    close(fd1);
    return;
}


void resume()
{
    //send resume value to master
    is_paused = 0;
    is_stopped = 0;
    char msg[1];
    msg[0] = 't';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    if (fd1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "resume tube could not be opened");
		exit(EXIT_FAILURE);
    }
    res = write(fd1, msg, 2);
    if (res == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "resume tube could not be written on");
		exit(EXIT_FAILURE);
    }
    printf(" sent resume\n");
    fflush(stdout);
    close(fd1);
    return;
}

void create_display(size_t rows, size_t cols, char (*display)[cols])
{
    //create and show empty display
    size_t i, j;
    
    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            //middle bar
            if (j == cols / 2)
            {
                display[i][j] = '|';
            }
            //lines for x axis
            else if (i == 1)
            {
                display[i][j] = '_';
            }
            //points everywhere else
            else
            {
                display[i][j] = '.';
            }
}

void set_position(double x, double y, size_t rows, size_t cols, char (*display)[cols])
{
    //set x position on display
    size_t i, j;

    //get closest int
    x = (int)round(x);
    y = (int)round(y);
    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            if (i == x)
            {
                //show x position
                if (j == y)
                {
                    display[i][j] = 'x';
                }
                else
                {
                    display[i][j] = '_';
                }
            }
            //else show normal display
            else if (j == cols / 2)
            {
                display[i][j] = '|';
            }
            else if (display[i][j] == '_')
            {
                display[i][j] = '.';
            }
            //remove old position
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

    //use 2 pipes to get position for x and y
    fd1 = open(fifomot1, O_RDONLY);
    if (fd1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "mot1 tube could not be opened");
		exit(EXIT_FAILURE);
    }
    
    fd2 = open(fifomot2, O_RDONLY);
    if (fd2 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "mot2 tube could not be opened");
		exit(EXIT_FAILURE);
    }
    resmot1 = read(fd1, linemot1, 80);
    if (resmot1 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "mot1 tube could not be read on");
		exit(EXIT_FAILURE);
    }
    resmot2 = read(fd2, linemot2, 80);
    if (resmot2 == -1){
        log_entry(logname, "ERROR",  __FILE__, __LINE__, "mot2 tube could not be read on");
		exit(EXIT_FAILURE);
    }

    //lines are of format "x, position"
    char format_string_mot1[80] = "%c,%f";
    char format_string_mot2[80] = "%c,%f";

    float value_get1 = 0.;
    char value_char1;
    float value_get2 = 0.;
    char value_char2;

    //get the values
    sscanf(linemot1, format_string_mot1, &value_char1, &value_get1);
    sscanf(linemot2, format_string_mot2, &value_char2, &value_get2);
    *x = value_get1;
    *y = value_get2;

    printf("x is %f \n", *x);
    fflush(stdout);
    printf("y is %f \n", *y);
    fflush(stdout);

    //close pipes
    close(fd1);
    close(fd2);
}

void show_display(size_t rows, size_t cols, char (*display)[cols])
{
    //print display on command shell
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
    int c;
    //actions for pause and reset
    switch (cmd)
    {
    case PAUSE:
        //pause
        pause_prog();
        while (is_paused == 1)
        {
            set_mode(1);
            if (c = get_key())
            {
                //resume if needed
                if (c == RESUME)
                {
                    action(c);
                }
            }
        }
        break;
    case STOP:
        //stop
        stop_prog();
        while (is_stopped == 1)
        {
            set_mode(1);
            if (c = get_key())
            {
                //resume if needed
                if (c == RESUME)
                {
                    action(c);
                }
            }
        }
        break;
    case RESUME:
        log_entry(logname, "INFO", __FILE__,  __LINE__, "Programs resumed");
        //send resume to master 
        resume();
        break;
    }
}

void sig_handler(int signo)
{
    //send reset if sigint is sent
    if (signo == SIGINT)
    {
        log_entry(logname, "INFO", __FILE__,  __LINE__, "Programs reset by user");
        //resume if the system is paused
        if (is_paused == 1){
            action(RESUME);
        }
        reset();
    }

    if (signo == SIGTSTP){
        log_entry(logname, "INFO", __FILE__,  __LINE__, "Programs kill by user");
        kill_prog();
    }

    //if the watchdog sent the reset
    if (signo == SIGUSR1){
        //resume if system is paused
        if (is_paused == 1){
            action(RESUME);
        }
    }
}

int main(int argc, char *argv[])
{
    strncpy(logname, argv[1], 39);
    logname[39] = 0;
    //signal handlers for resets
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGINT\n");
        }
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGUSR1\n");
        }
    if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGTERM\n");
        }

	log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Execution started");

    size_t rows = 0;
    size_t cols = 0;

    rows = 15;
    cols = 15;

    char display[rows][cols];

    //display initializing

    create_display(rows, cols, display);
    show_display(rows, cols, display);

    //pipes initializing

    char *fifomot1 = "/tmp/motor";
    mkfifo(fifomot1, 0666);
    char *fifomot2 = "/tmp/motor2";
    mkfifo(fifomot2, 0666);

    double x = cols + 1;
    double y = rows + 1;

    int c;

    printf("initialization complete \n");
    fflush(stdout);

    printf("push S for emergency stop, P to pause, R to resume and CTRL^C to reset \n");
    fflush(stdout);

    while (1)
    {
        //get pause if needed
        set_mode(1);
        if (c = get_key())
        {
            if (c == PAUSE)
            {
                log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Programs in pause");
                action(c);
            }
            else if (c == RESUME){
                printf("Cannot resume, system is not STOPPED or PAUSED \n");
                fflush(stdout);
                printf("push S for emergency stop, P to pause, R to resume and CTRL^C to reset \n");
                fflush(stdout);
            }
            else if (c == STOP){
                log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Emergency stop");
                action(c);
            }
            else{
                printf("Invalid Command \n");
                fflush(stdout);
                printf("push S for emergency stop, P to pause, R to resume and CTRL^C to reset \n");
                fflush(stdout);
            }
        }
        
        //get and set position, then show display on screen
        get_position(&x, &y, rows, cols, display, fifomot1, fifomot2);
        set_position(x, y, rows, cols, display);
        show_display(rows, cols, display);
    }
}
