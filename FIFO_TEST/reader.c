#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>

#define FIRST_WRITER "./tmp/first"

#define SECOND_WRITER "./tmp/second"

#define BUFFER 1024

int main(int argc, char *argv[]) {

    char first_in[BUFFER];
    char second_in[BUFFER];

    mkfifo(FIRST_WRITER, 0666);
    
    mkfifo(SECOND_WRITER, 0666);

    printf("Welcome to server.\n");

    int first_fd=open(FIRST_WRITER, O_RDONLY);
    int second_fd = open(SECOND_WRITER, O_RDONLY); 
    
    printf("First: %d, Second: %d\n", first_fd, second_fd);

    if (first_fd==-1 || second_fd==-1) {
        perror("open error");
        exit(-1);
    }

    int nfd = first_fd > second_fd ? first_fd : second_fd;
    fd_set  tset, master;
    FD_ZERO(&master);
    FD_SET(first_fd, &master);
    FD_SET(second_fd, &master);
    tset = master;
    
    while (select(nfd+1, &tset, NULL, NULL, NULL)>0) {
        if (FD_ISSET(first_fd, &tset)) {
            if (read(first_fd, first_in, BUFFER) > 0) {
                printf("Message from First: %s\n", first_in);
                memset(first_in, '\0', BUFFER); 
            }
            tset = master;
            continue;
        }
        
        if (FD_ISSET(second_fd, &tset)) {
            if (read(second_fd, second_in, BUFFER) > 0) {
                printf("Message from Second: %s\n", second_in);
                memset(second_in, '\0', BUFFER); 
            }
            tset = master;
            continue;
        }
    }
    printf("End\n");
    return 1;
}