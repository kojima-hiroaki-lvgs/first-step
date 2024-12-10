#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define HOME getenv("HOME")

char* read_command(){
    char pathname[512];
    getcwd(pathname, 512);
    printf("%s $ ",pathname);

    char* command = calloc(1024,sizeof(char));
    fgets(command,1024,stdin);
    command[strlen(command)-1] = '\0';
    return command;
}

char** cut_command(char* command){
    char *cut;
    char **cutRoom = calloc(128, sizeof(char*));
    char *delimiter = " ";
    cut = strtok(command, delimiter); //空白切り
    int cmd_index = 0;
    while (cut != NULL){
        cutRoom[cmd_index] = cut;
        cmd_index++;
        cut = strtok(NULL, delimiter);
    }
    cutRoom[cmd_index] = NULL;
    return cutRoom;
}

char** redirect_command(char** command_tmp){
    int fd, flag = 0, removeIndex = 0;
    char** newCommand_tmp = calloc(128, sizeof(char*));
    int i = 0;
    while(command_tmp[i] != NULL){
        if (strcmp(command_tmp[i],"<") == 0){
            fd = open(command_tmp[i+1], O_RDONLY);
            dup2(fd,0);
            close(fd);
            flag = 1;
        } else if (strcmp(command_tmp[i],">") == 0){
            fd = open(command_tmp[i+1], O_WRONLY|O_CREAT|O_TRUNC,0644);
            dup2(fd,1);
            close(fd);
            flag = 1;
        } else if (strcmp(command_tmp[i],">>") == 0){
            fd = open(command_tmp[i+1], O_WRONLY|O_CREAT|O_APPEND,0644);
            dup2(fd,1);
            close(fd);
            flag = 1;  
        } else if (flag == 1){ // リダイレクトの次(txtファイル)
            flag = 0;
        } else { //リダイレクトがない場合
            newCommand_tmp[removeIndex] = command_tmp[i];
            removeIndex++;
        }
        i++;
    }
    return newCommand_tmp;
}

void child_process(int pipe_flag, int pipe_count, char** newCommand_tmp, int pipe_fd[128][2]){
    if (pipe_flag == 1){
            dup2(pipe_fd[0][1],1);
            close(pipe_fd[0][1]);
        } else if (pipe_flag > 1 && pipe_flag < pipe_count + 1){
            dup2(pipe_fd[pipe_flag - 2][0],0);
            dup2(pipe_fd[pipe_flag - 1][1],1);
            close(pipe_fd[pipe_flag - 2][1]);
            close(pipe_fd[pipe_flag - 1][1]);
        } else if (pipe_flag == pipe_count + 1){
            dup2(pipe_fd[pipe_flag - 2][0],0); 
            close(pipe_fd[pipe_flag - 2][1]);
        } 
        printf("%s\n",newCommand_tmp[0]);
        printf("%s\n",newCommand_tmp[1]);
        printf("%s\n",newCommand_tmp[2]);
        execvp(newCommand_tmp[0],newCommand_tmp);
        printf("command not found: %s\n",newCommand_tmp[0]);
        exit(EXIT_FAILURE);
}

int execute_command(char** command_tmp, int cutRoom_index, int tmp_index, int input_fd, int output_fd, int pipe_flag, int pipe_fd[128][2], int pipe_count){
    char** newCommand_tmp = redirect_command(command_tmp);
    int status = 0;

    if(newCommand_tmp[0] == NULL) {
        //処理なし(入力ない時)
    } else if (strcmp(newCommand_tmp[0],"cd") == 0){
        if(tmp_index == 1){
            chdir(HOME); 
        } else {
            chdir(newCommand_tmp[cutRoom_index-1]);
        }
    } else if(strcmp(newCommand_tmp[0],"exit") == 0){
        exit(EXIT_SUCCESS);
    } else { //外部コマンドの処理
        pid_t pid = fork();
        if (pid == 0){
            child_process(pipe_flag, pipe_count,newCommand_tmp, pipe_fd);
        } else if (pid > 0) { //親プロセス
            waitpid(pid, &status, 0);
        } else {
            printf("fork error\n");
        }
    }
    return status;
}

void process_command(char** cutRoom){
    int cutRoom_index = 0, tmp_index = 0;
    char** command_tmp = calloc(128, sizeof(char*));
    int pipe_fd[128][2], pipe_flag = 0, pipe_count = 0;
    int input_fd = dup(0);
    int output_fd = dup(1);

    while (cutRoom[cutRoom_index] != NULL){
        if (strcmp(cutRoom[cutRoom_index],"|") == 0){
            pipe_count++;
            cutRoom_index++;
        } 
        cutRoom_index++;
    }
    cutRoom_index = 0; //後で使うので初期化

    while (cutRoom[cutRoom_index] != NULL){
        printf("cutroom_index: %d\n", cutRoom_index);
        if (strcmp(cutRoom[cutRoom_index],"&&") == 0){ //&&の処理
            if (pipe_count != 0){ //これpipe1つ &&2つの時に&&の2回目も通っちゃう
                printf("kokodayo\n");
                pipe_flag = pipe_flag + 1;
                execute_command(command_tmp, cutRoom_index, tmp_index, input_fd, output_fd, pipe_flag, pipe_fd, pipe_count);
                close(pipe_fd[pipe_flag - 3][0]);
                close(pipe_fd[pipe_flag - 2][1]);
            }
            command_tmp[tmp_index] = NULL;
            if (execute_command(command_tmp, cutRoom_index, tmp_index, input_fd, output_fd, pipe_flag, pipe_fd, pipe_count) != 0){
                //return; //存在しないコマンドを入力した場合
            } 
            tmp_index = 0;
        } else if (strcmp(cutRoom[cutRoom_index],"|") == 0){
            pipe_flag++;
            if (pipe_flag == 1){ //初回
                pipe(pipe_fd[0]);
                execute_command(command_tmp, cutRoom_index, tmp_index, input_fd, output_fd, pipe_flag, pipe_fd, pipe_count);
                close(pipe_fd[0][1]);
                tmp_index = 0;
                if (pipe_count == 1){
                    cutRoom_index++; 
                    continue;
                }
            } else if (pipe_flag > 1){ //2回目以降
                pipe(pipe_fd[pipe_flag - 1]);
                execute_command(command_tmp, cutRoom_index, tmp_index, input_fd, output_fd, pipe_flag, pipe_fd, pipe_count);
                close(pipe_fd[pipe_flag - 2][0]);
                close(pipe_fd[pipe_flag - 1][1]);
                tmp_index = 0;
            } 
        } else {
            printf("文字\n");
            command_tmp[tmp_index] = cutRoom[cutRoom_index];
            tmp_index++;
        }
        cutRoom_index++;           
    }

    if (pipe_flag == 0){ //パイプ無い時の
        command_tmp[tmp_index] = NULL;
        execute_command(command_tmp, cutRoom_index, tmp_index,input_fd, output_fd, pipe_flag, pipe_fd, pipe_count);
    } else {
        pipe_flag = pipe_flag + 1;
        command_tmp[tmp_index] = NULL;
        execute_command(command_tmp, cutRoom_index, tmp_index,input_fd, output_fd, pipe_flag, pipe_fd, pipe_count);
        //if (pipe_flag == 2){
            // close(pipe_fd[pipe_flag - 3][0]);
            // close(pipe_fd[pipe_flag - 2][1]);
            close(input_fd);
            close(output_fd);
        //}
    }        
}

void shell(){
    while(true){
        char* command = read_command();
        char** cutRoom = cut_command(command);
        process_command(cutRoom);
        free(command);
    }
}

int main(){
    shell();
    return 0;
}
    