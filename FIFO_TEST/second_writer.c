#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>

#define BUFFER 1024

#define SECOND_WRITER "./tmp/second"

int main(int argc, char *argv[]) {

    char input[BUFFER]={0};
    int out_fd=open(SECOND_WRITER, O_WRONLY);

    if (out_fd==-1) {
        perror("open error");
    }
    
    printf("Second Writer\n");

    while (1) {
        
        fgets(input, BUFFER, stdin);
        
        if (input[strlen(input)-1]=='\n') {
            input[strlen(input)-1]='\0';
        }

        if (strcmp(input, "Quit")==0) {
            printf("Bye!");
            break;
        }

        if (write(out_fd, input, strlen(input))==-1) {
            perror("write error");
        }
    }

    return 1;
}