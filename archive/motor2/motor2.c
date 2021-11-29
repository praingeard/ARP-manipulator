#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "../logarp/logarp.h"

//named pipe to send position to the display process
char *fifomot2 = "/tmp/motor2";
double y = 0.0;
// Increment of the position at each pass in the while loop
// Its value is modified with the given commands
int step = 0;

//To store the logfile name
char logname[40] = "log.txt";


// Reads the commands sent by cmd_shell
void read_input(int *step)
{
    //Char array to store the command that will be read on the pipe
    char recieved[1];
    int fd1;
    int res;

    //Named pipe written on by cmd_shell
    char *myfifo = "/tmp/x_motor";
    mkfifo(myfifo, 0666);
    //Open the pipe en this end
    fd1 = open(myfifo, O_RDONLY);

    // Read the command, which is always one character long 
    res = read(fd1, recieved, 2);
    if (res < 0)
    {
        log_entry(logname, "ERROR", __FILE__,  __LINE__, "Motor2 failed to recieve command");
    }

    //Close the pipe
    close(fd1);

    // According to the character, we update the value of step
    //P stands for plus, m for minus, s for stop
    if (recieved[0] == 'p')
        *step = 1;
    else if (recieved[0] == 'm')
        *step = -1;
    else if (recieved[0] == 's')
        *step = 0;

    return;
}

//This function sends the updated horiziontal position to the display process
void write_position(double y, char *fifomot2)
{
    int fd1;
    //will contain the message to be send
    char input_string[80];
    char format_string[80] = "%c, %f";
    //the positon is added to the message string
    sprintf(input_string, format_string,'y', y);
    
    //Use of a named pipe to write
    fd1 = open(fifomot2, O_WRONLY);    
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}


//Modifies the value of the position variable using step
void set_position(int *step, double *y)
{
    // random error between 0 and 1
    double err = (double)rand() / (double)RAND_MAX;
    // we modifiy the error so that it is between -0.1 and 0.1
    err = 0.2 * err - 0.1;

    //Update the posiiton with a step increment and an error
    *y = *y + *step * (1 + err);

    //The systems has physical limits, so there is a saturation once these limits are met
    if (*y < 0)
        *y = 0;
    
    if (*y > 14)
        *y = 14;
    
    return;
}

// To communicate with master, to quit all process
void kill_prog()
{
    char msg[1];
    msg[0] = 'q'; //q for quit
    int fd1, res;
    //creation of the named pipe
    char *myfifo = "/tmp/reset";
    mkfifo(myfifo, 0666);
    
    //Use of a named pipe for writing
    fd1 = open(myfifo, O_WRONLY);
    res = write(fd1, msg, 2);
    
    close(fd1);
    return;
}


// Handles all the signals
void sig_handler(int signo)
{
    
    if (signo == SIGINT){
        // We received a reset command
        // The motors now have to put the hoist back in its orignal position
        
        // The original position is 0,0, that means that whereever the hoist is
        // the use of a negative step will allow to reach the origin
        step = -1;

        // We need to communicate with the display
        mkfifo(fifomot2, 0666);
        char *fifomot1 = "/tmp/motor";
        mkfifo(fifomot1, 0666);
        int fd1;
        while(1){
            if (y < 0.1){
                //origin position is reached with a satisfiable precision
                break;
            }
            
            // To avoid being stuck in the display, in case motor1 already finished its task
            // We open and close the named pipe used by motor1
            fd1 = open(fifomot1, O_WRONLY);
            close(fd1);

            // As long as needed, we modify the position of the hoist
            set_position(&step,&y);            
            write_position(y, fifomot2);
            sleep(1);
        }

        //When reset is done, we want the mouvment to stop on this axis       
        step = 0;
    }
    if (signo == SIGUSR1)
    {
        //The system recieved a "pause" signal
        pause();
    }
    if (signo == SIGUSR2){
        //The system recieved a "resume" signal
        fflush(stdout);
    }
     if (signo == SIGTSTP){
        // If it recieved ctrl+C : makes sure to exit all the processes
        kill_prog();
    }
}



int main(int argc, char *argv[])
{
    // Stores the logfile name to be reused in other functions if need be
    strncpy(logname, argv[1], 39);
    logname[39] = 0;

	log_entry(argv[1], "INFO", __FILE__,  __LINE__, "Execution started");

    // declaration of the signals handler
    if (signal(SIGINT, sig_handler) == SIG_ERR){
        log_entry(argv[1], "ERROR", __FILE__,  __LINE__, "Can't catch SIGINT");
        }
     if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        {
            log_entry(argv[1], "ERROR", __FILE__,  __LINE__, "Can't catch SIGUSR1");
        }
        if (signal(SIGUSR2, sig_handler) == SIG_ERR)
        {
            log_entry(argv[1], "ERROR", __FILE__,  __LINE__, "Can't catch SIGUSR2");
        }
         if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        {
            log_entry(argv[1], "ERROR", __FILE__,  __LINE__, "Can't catch SIGSTP");
        }


    //initialisation of the random generator
    time_t t;
    srand((unsigned)time(&t));
    mkfifo(fifomot2, 0666);

    // infinite loop
    while (1)
    {
        //Read the commands from the user
        read_input(&step);
        //Update position of the hoist
        set_position(&step, &y);
        //Communicate new position to the display
        write_position(y, fifomot2);
        sleep(1);
    }
}
