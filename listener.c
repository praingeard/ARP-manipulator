#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{

    while (1)
    {
        char recieved[1] = "";
        int fd1;
        int res;
        char *myfifo = argv[1];
        mkfifo(myfifo, 0666);
        fd1 = open(myfifo, O_RDONLY);
        res = read(fd1, recieved, 1);
        printf("%s", recieved);
        printf("confirmation");
        
        close(fd1);
        sleep(1);
    }
}
