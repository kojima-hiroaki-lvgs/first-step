#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>
#include <sys/types.h>  



char *builtin_str[] = {
    "pwd"
    "cd"
};

int builtin_num(){  //このスラッシュなんだ？あと元々**argv入れてたけど意味なさそうだから消した
    return sizeof(builtin_str) / sizeof(char*);
};
int builtin_pwd(){
    char path[1024];
    if(!getcwd(path, sizeof(path))){
        return -1;
    }
    printf("%s\n", path);
    return 0;
}

int builtin_cd(char **argv){
    if (argv[1] == NULL){
        fprintf(stderr, "expected argument to");
    }
    return 0;
}

int (*builtin_func[])(char **) = { //なんでアドレスとってるんだろう
    &builtin_pwd,
};

char* read_command(void) {
    char* Read = malloc(1024);
    printf("shell¥\n");
    
    if (fgets(Read,1024,stdin) == NULL){
        if (feof(stdin)) {
            fprintf(stderr,"入力の終了を検出");
            exit(EXIT_SUCCESS);
        } else {
            perror("error");
            exit(EXIT_FAILURE);
        }
    }
    Read[strlen(Read)-1] = '\0'; //コマンドの長さの-1のところ(\n)を置き換える
    return Read;
}

char* cut_command(char* read, char* cutRoom[1024]){
    char *cut;
    char *args;
    int i=0;
    
    cut = strtok(read," ");
   // printf("%s\n",cut);
    while (cut != NULL){
        cutRoom[i] = cut;
        args = cutRoom[i];
        i++;
        cut = strtok(NULL," ");
    }

    printf("%s\n",cutRoom[0]);
    return cutRoom[1024];
}

int execute_INcommand(char *argv[], int builtin_num){
    int i;
    for (i=0; i<builtin_num; i++){
        if (strcmp(argv[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(argv);
        }
    }
    return 1;
}

int execute_EXcommand(char *argv[]){
    pid_t pid, wpid;
    int status;
    


    pid = fork();
    if (pid == 0){
        if(execvp(argv[0], argv) == -1){ //ここでエラー出てる
            perror("command failed");
        } 
        exit(EXIT_FAILURE);
    } else if (pid < 0){
        perror("proccess failed");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int main(int argc, char *argv[]){
    char* read;
    char* cut;
    char* cutRoom[1024]; 
    int builtin_num;   
    int INcommand;
    
    while(true){
        read = read_command();    
        cut = cut_command(read,cutRoom);
        memcpy(argv,cutRoom,36);
        INcommand = execute_INcommand(argv,builtin_num);

       // strcpy(*argv,*cutRoom); //strcopyだと\0で終わってoption読み取られない
        printf("%sあああ\n",argv[0]);
        printf("%sあああ\n",argv[1]);
        printf("%sあああ\n",argv[2]);
        printf("%sいいい\n",cutRoom[0]);
        printf("%sいいい\n",cutRoom[1]);
        printf("%sいいい\n",cutRoom[2]);

        if (INcommand == 1)
        execute_EXcommand(argv);
        memset(*cutRoom, '\0', 1024);
        memset(*argv, '\0', sizeof(argv[1024]));
    }
};