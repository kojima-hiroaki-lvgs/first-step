
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <wordexp.h>
#include <signal.h>

int main() {
int byte_num, pipe_fd[2];
char buf[512] 


pipe(pipe_fd[2]);
pid_t pid = fork();
if(pid == 0){
    close(pipe_fd[1]);
    read(pipe_fd[0], buf, 512);
} else {
    close(pipe_fd[0]);
    write
    wait();
}



}