/* The user has at its disposal 6 commands to control the mouvement of the hoist
on the x axis : left, right, stop
on the z axis : up, down, stop

In order to have a system easy to use, the role of the different keys to use HAS
to be displayed upon execution

for now : let's use the following keys :
q : left
s : stop horizontal mouvement
d : right

8 : up
5 : stop vertical mouvement
2 : down

les touches de commandes peuvent-elles être passées en arguments ?
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

/*set as a global variable to allow quick change of settings ==> Which for now is not the cas, failure to use it with switch*/
const int commands[6] = {113, 115, 100, 50, 53, 56}; // values for q, s, d and 2, 5, 8
#define LEFT 113
#define STOPX 115
#define RIGHT 100
#define DOWN 50
#define STOPZ 53
#define UP 56

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
    for (int idx = 0; idx < 6; idx++)
    {
        if (pressed_key == cmds[idx])
            return 1;
    }
    return 0;
}

void action(int cmd)
{
    /*sends the command to the right motor*/
    int fd;
    char *fifo;
    char msg[1];

    switch (cmd)
    {
    case LEFT:
        fifo = "/tmp/x_motor";
        msg[0] = 'p';
        break;
    case STOPX:
        fifo = "/tmp/x_motor";
        msg[0] = 's';
        break;
    case RIGHT:
        fifo = "/tmp/x_motor";
        msg[0] = 'm';
        break;
    case DOWN:
        fifo = "/tmp/z_motor";
        msg[0] = 'p';
        break;
    case STOPZ:
        fifo = "/tmp/z_motor";
        msg[0] = 's';
        break;
    case UP:
        fifo = "/tmp/z_motor";
        msg[0] = 'm';
        break;
    }

    mkfifo(fifo, 0666);
    fd = open(fifo, O_WRONLY);
    write(fd, msg, strlen(msg) + 1);
    close(fd);
    
}


int main()
{
    int c;

    while (1)
    {
        set_mode(1);
        if (c = get_key())
        {
            /*if I am not completly lost, this means the user pressed a key*/
            /*We have to check wether it is a command or not*/
            if (is_command(c, commands))
            { //communication avec les moteurs, peut-être faut-il aussi se souvenir de la valeur précédente
                printf("action\n");
                fflush(stdout);
                action(c);
            }
            else
            {
                printf("This is not a correct command key, use :\nq: left, s: stop horizontal, d: right\n2: down, 5: stop vertical, 8: up\n");
                fflush(stdout);
            }
        }
    }
}
