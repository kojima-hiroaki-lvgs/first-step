#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int pipe_fds[2];
    pipe(pipe_fds);
    if (fork() == 0) {
        //write(pipe_fds[1], "Hello World.", 12);
        dup2(pipe_fds[1],1);
        printf("aaa\n");
        exit(0);
    } else {
        char buf[1024] = {0};
        //close(pipe_fds[1]);
        dup2(pipe_fds[0],0);
        //read(pipe_fds[0], buf, sizeof(buf));
        
    }
    return 0;
}