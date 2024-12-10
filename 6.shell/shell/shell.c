#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>
#include <sys/types.h>  

static void child(char *argv[]){
    execvp(argv[0],argv); 
  //  exit(EXIT_SUCCESS);
}

static void parent(pid_t pid_child, int status){
}

int main(int argc, char *argv[]) {
    char line[1024];
    int status;

    while (true) {
        printf("shell¥\n");
        fgets(line, 1024, stdin);
        line[strlen(line)-1] = '\0'; //改行がある位置に終端\0を代入する
        char *argv[] = {line, NULL};
        //printf("%lu\n",strlen(line));

        pid_t pid; //forkの戻り値を保存する変数
        pid = fork(); //子プロセスを作る
        if (pid == 0){ //子プロセス
            child(argv);
        } else if (pid == -1){
            err(EXIT_FAILURE,"fork() failed");
        } else {
            parent(pid, status);
        }
        
    };
}