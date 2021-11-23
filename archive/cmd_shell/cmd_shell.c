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
#include <time.h>

#include "../logarp/logarp.h"

// values for q, s, d and 2, 5, 8
#define LEFT 113
#define STOPX 115
#define RIGHT 100
#define DOWN 50
#define STOPZ 53
#define UP 56
// We store them in an array for convenience 
const int commands[6] = {LEFT, STOPX, RIGHT, DOWN, STOPZ, UP};



void set_mode(int want_key)
// Auxilliary function needed to get the input from the keyboard
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
// Check wether the user has pressed a key on the keyboard, monitoring the terminal
{
    int c = 0;
    //tv is a deadline, we set it at zero because we do not want to wait with select
    struct timeval tv;
    fd_set fs;
    tv.tv_usec = tv.tv_sec = 0;
    int ret_val; 


	//STDIN_FILENO corresponds to the input terminal
    FD_ZERO(&fs);
    FD_SET(STDIN_FILENO, &fs);
    ret_val = select(STDIN_FILENO + 1, &fs, 0, 0, &tv);

	//We check if there has been an error
    if (ret_val == -1)
        perror("select()");
    else if (ret_val)
    {
        // (FD_ISSET(STDIN_FILENO, &fs)) will be true
        // We update c with the numerical code associated with the key
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


void noaction(char *fifo)
{
	/* In case the pressed key is no a command key, we still send a message
	 * This is to avoid having the other process blocked by an pipe opened
	 * on one side only */
	 
    int fd;  
    char msg[1];
    // o means the motors shoud not change
    msg[0] = 'o';
    int res;

	// opening of the named pipe
    mkfifo(fifo, 0666);
    fd = open(fifo, O_WRONLY);
    res = write(fd, msg, strlen(msg) + 1);

	//Check if no error occured
    if (res < 0){
        printf("no value\n");
        fflush(stdout);
    }
    close(fd);
    
}


void action(int cmd, char *name_cmd)
{
    /*sends the command to the right motor*/
    int fd;
    char *fifo, *fifoNoaction; //We will send a command on one pipe and just 'o' on the other
    char msg[1];

    switch (cmd)
    {
	//The commands left, right and stopx are for the motor 1
    case LEFT:
		//fifo is the pipe that will send the command
        fifo = "/tmp/x_motor";
        // fifoNoaction is the one sending an 'o' to say nothing is to be changed
        fifoNoaction = "/tmp/z_motor";
        // We send a letter to the motor, 'm' for minus, 's' for stop, 'p' for plus
        msg[0] = 'm';
        strncpy( name_cmd, "LEFT", 39);
        name_cmd[39] = 0;
        break;
    case STOPX:
        fifo = "/tmp/x_motor";
        fifoNoaction = "/tmp/z_motor";
        msg[0] = 's';
        strncpy( name_cmd, "STOPX", 39);
        name_cmd[39] = 0;
        break;
    case RIGHT:
        fifo = "/tmp/x_motor";
        fifoNoaction = "/tmp/z_motor";
        msg[0] = 'p';
        strncpy( name_cmd, "RIGHT", 39);
        name_cmd[39] = 0;
        break;
        
        //The commands down, up and stopz are for the motor 2
    case DOWN:
        fifo = "/tmp/z_motor";
        fifoNoaction = "/tmp/x_motor";
        msg[0] = 'p';
        strncpy( name_cmd, "DOWN", 39);
        name_cmd[39] = 0;
        break;
    case STOPZ:
        fifo = "/tmp/z_motor";
        fifoNoaction = "/tmp/x_motor";
        msg[0] = 's';
        strncpy( name_cmd, "STOPZ", 39);
        name_cmd[39] = 0;
        break;
    case UP:
        fifo = "/tmp/z_motor";
        fifoNoaction = "/tmp/x_motor";
        msg[0] = 'm';
        strncpy( name_cmd, "UP", 39);
        name_cmd[39] = 0;
        break;
    }

	//Now that the messages and the names of the pipes are set, we can call open them
    int res;
    mkfifo(fifo, 0666);
    fd = open(fifo, O_WRONLY);
    
    
    res = write(fd, msg, strlen(msg) + 1);
    if (res < 0){
        printf("no value\n");
        fflush(stdout);
    }
    
    close(fd);
    
    //We use noaction to send 'o' to the other motor
    noaction(fifoNoaction);
    
}

void kill_prog()
{
    char msg[1];
    msg[0] = 'q';
    int fd1, res;
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    printf(" sent kill\n");
    fflush(stdout);
    close(fd1);
    return;
}

void sig_handler(int signo)
{
    if (signo == SIGTSTP || signo == SIGINT){
        kill_prog();
    }
}



int main(int argc, char *argv[])
{
    if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGTERM\n");
        }
     if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            printf("\ncan't catch SIGINT\n");
        }

    int c;
    char name_cmd[40] = "NOTHING";
    
    // Entry to the logfile whose named was created by master and contained in argv[1]
	log_entry(argv[1], "NOTICE", __FILE__, __LINE__, "Execution started");

    while (1)
    {
        set_mode(1);
        
        // Continously checking for a user input
        if (c = get_key())
        {            
            /*We have to check wether it is a command or not*/
            if (is_command(c, commands))
            { 
				action(c, name_cmd);
				char msg[] = "The following command was pressed : ";
				strcat(msg,name_cmd);
				log_entry(argv[1], "INFO", __FILE__, __LINE__, msg);
				printf("You pressed the command %s \n", name_cmd);
                fflush(stdout);
            }
            else
            {
				// print the working commands
                printf("This is not a correct command key, use :\nq: left, s: stop horizontal, d: right\n2: down, 5: stop vertical, 8: up\n");
                fflush(stdout);
                noaction("/tmp/x_motor");
                noaction("/tmp/z_motor");
            }
        }
        else
        {
            noaction("/tmp/x_motor");
            noaction("/tmp/z_motor");
        }

        sleep(2);
    }
}
