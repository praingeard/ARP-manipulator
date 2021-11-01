#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main(){

    double x = 0;

    char *fifomot1 = "/tmp/motor1";
    mkfifo(fifomot1, 0666);

    while(1){
        read_input(&x);
        set_position(&x);
        write_position(x,&fifomot1);
        sleep(1);
    }
}

void read_input(double *x){

}

void write_position(double x, char fifomot1){
    int fd1;
    char input_string[80];
    char format_string[80]="%d";
    sprintf(input_string, format_string, x);
    fd1 = open(fifomot1, O_WRONLY);
    printf("writing value %d", x);
    write(fd1, input_string, strlen(input_string) + 1);
    close(fd1);
}

void set_position(double *x){
    *x = *x + 1;
}