#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main(){

    double y = 0;

    char *fifomot2 = "/tmp/motor2";
    mkfifo(fifomot2, 0666);

    while(1){
        read_input(&y);
        set_position(&y);
        write_position(y,&fifomot2);
        sleep(1);
    }
}

void read_input(double *y){

}

void write_position(double y, char fifomot2){
    int fd1;
    char input_string[80];
    char format_string[80]="%d";
    sprintf(input_string, format_string, y);
    fd1 = open(fifomot2, O_WRONLY);
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}

void set_position(double *y){
    *y = *y + 1;
}