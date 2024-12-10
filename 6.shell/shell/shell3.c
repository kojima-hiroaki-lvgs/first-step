// cd chat && ls -l だけを限定的に実装

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>
#include <sys/types.h>  

#define PATHNAME_SIZE 512

char *builtin_str[] = {
    "pwd","cd"
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

int builtin_cd(char *argv[]){
    char pathname[PATHNAME_SIZE]; 
    chdir(argv[1]); // チェンジディレクトリ
    return 0;
}

int (*builtin_func[])(char **) = { //なんでアドレスとってるんだろう
    &builtin_pwd,
    &builtin_cd,
};

char* read_command(char pathname[512]) {
    char* Read = malloc(1024);
    getcwd(pathname, PATHNAME_SIZE);
    printf("%s ~ ☺︎", pathname);
    
    if (fgets(Read,1024,stdin) == NULL){
        if (feof(stdin)) {
            fprintf(stderr,"入力の終了を検出");
            exit(EXIT_SUCCESS);
        } else {
           // perror("error");
            exit(EXIT_FAILURE);
        }
    }
    Read[strlen(Read)-1] = '\0'; //コマンドの長さの-1のところ(\n)を置き換える
    return Read;
}

char* cut_command(char* read, char* cutRoom[1024], int *cutCount){
    char *cut;
    char *tmp;
    int i=0 ;

    
    cut = strtok(read," ");
   // printf("%s\n",cut);
    while (cut != NULL){
        cutRoom[i] = cut;
        tmp = cutRoom[i];
        i++;
        cut = strtok(NULL," ");
    }
    *cutCount = i; //iが区切りの数&&引数がすでにあるため、cutCountでポインタ渡ししている
    return cutRoom[1024];
}



int execute_INcommand(char *cmdBox[], int builtin_qty,int cutCount,int *branchNum){
    int i;
    //printf("%dcutCount\n",cutCount);
    for (i=0; i<builtin_qty; i++){
        if ((strcmp(cmdBox[0], "cd") == 0) && (cutCount == 1)){ //cdだけ打ったら
            chdir("/Users/hiroaki.kojima");
            return 0; //ここで0返さないと外部コマンド実行されちゃう
        } else if (strcmp(cmdBox[0], builtin_str[i]) == 0){ //内部コマンドのリストと合致していたら
            if(strcmp(cmdBox[2],"&&") == 0){
                *branchNum = i;
                return 2;
            }            
            //追加部分
            return (*builtin_func[i])(cmdBox); //指定のコマンドを実行
        }
    }
    return 1;
}

int execute_EXcommand(char *cmdBox[],int cutCount, int var){
    pid_t pid, wpid;
    int status;
    if (var == 0){
    cmdBox[cutCount] = NULL; //ここを変数にすることでオプションの数に応じてNULLをおけるようになった
    } else if (var == 1){
    cmdBox[2] = NULL;
    }

    pid = fork();
    printf("%s%s\n",cmdBox[0],cmdBox[1]);
    if (pid == 0){
        printf("%sはいいいいい\n",cmdBox[0]);
        if(execvp(cmdBox[0], cmdBox) == -1){ //ここでエラー出てる!!!!!!!!!!
            perror("command failed");
        } 
        printf("？？？？\n");
        exit(EXIT_FAILURE);
    } else if (pid < 0){
        perror("proccess failed");
    } else {
        printf("abcdefg\n");
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 0;
}

int main(int argc, char *argv[]){
    int branchNum; 
    char* read;
    char* cut;
    char* cutRoom[1024]; 
    char* cmdBox[1024];
    char tmp[1024][64];
    char *p;
    int builtin_qty;   
    int INcommand;
    int cutCount;
    int andLoc;
    int i = 0;
    char pathname[PATHNAME_SIZE]; 
    chdir("/Users/hiroaki.kojima");
    getcwd(pathname, PATHNAME_SIZE);
   

    memset(*argv,'\0',1024);

    while(true){
        builtin_qty = builtin_num();
        read = read_command(pathname);    
        cut = cut_command(read,cutRoom,&cutCount);
        memcpy(cmdBox,cutRoom,1024);
        int i;
        int j;
        int var;
        
        INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum);

        if(INcommand == 2){
            (*builtin_func[branchNum])(cmdBox);
            //memcpy(tmp[0],cmdBox[3],12); //cmdboxの3を&&の位置の変数を入れてfor文でcopy
            for(i=0; i<(cutCount-2); i++){
                memcpy(tmp[i],cmdBox[i+3],12);
            }

            printf("%s yeahaaaaa\n",tmp[0]);
            printf("%s yeahaaaaa\n",tmp[1]); 
            
            memset(*cmdBox,'\0',1024);
            for(j=0; j<(cutCount-2); j++){
                memcpy(cmdBox[j],tmp[j],64);
            }

            printf("%s yeahaaaaa\n",cmdBox[0]); 
            printf("%s yeahaaaaa\n",cmdBox[1]); 

            INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&andLoc);
            var = 1; //ここもマジックナンバーなの嫌かも
            if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,var);
            INcommand = 3; //ここも初期値というか決めておくべき
            printf("ex抜けたよ\n");
        }
        //追加部分

        var = 0; //マジックナンバーやだ！
        if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,var);
        memset(*cmdBox, '\0', sizeof(cmdBox));
        memset(*cutRoom, '\0', sizeof(cutRoom));
    }
};