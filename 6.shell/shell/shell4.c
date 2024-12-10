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

int execute_INcommand(char *cmdBox[], int builtin_qty,int cutCount,int *branchNum, int *andLoc,int *var){
    int i;
    int k;
    //printf("%dcutCount\n",cutCount);
    printf("%s今のコマンドボックス\n",cmdBox[0]);
    for (i=0; i<builtin_qty; i++){
        for (k=0; k<cutCount; k++){
            if ((strcmp(cmdBox[0], "cd") == 0) && (cutCount == 1) || strcmp(cmdBox[1], "&&") == 0){ //cdだけ打ったら
                chdir("/Users/hiroaki.kojima");
                printf("%s\n",cmdBox[1]);
                if (strcmp(cmdBox[1], "&&") == 0){
                    *var = 2;
                    printf("きたよ\n");
                    break;
                } else {
                     return 0; //ここで0返さないと外部コマンド実行されちゃう
                }
            } else if ((strcmp(cmdBox[0], builtin_str[i]) == 0) && (strcmp(cmdBox[k],"&&") != 0)){ //内部コマンドのリストと合致 & &&が含まれていなかったら
                *branchNum = i;
                printf("&&ないよ\n");
                return (*builtin_func[i])(cmdBox);
            }  else if (strcmp(cmdBox[k],"&&") == 0){ // &&が含まれていたら
                *andLoc = k;
                *branchNum = i;
                printf("%dナンバー\n",i);
                printf("&&あるよ\n");
                *var = 2;
            }   
        }  
    } 
   
    printf("外部コマンド実行\n");
    return 1; //内部コマンドないため、外部コマンドを実行
}    
    
    


int execute_EXcommand(char *cmdBox[],int cutCount, int var){
    pid_t pid, wpid;
    int status;
    if (var == 0){//初回の時の処理
    cmdBox[cutCount] = NULL; //ここを変数にすることでオプションの数に応じてNULLをおけるようになった
    } else if (var == 1){
    cmdBox[1] = NULL;
    }

    pid = fork();
    
    if (pid == 0){
        //printf("%sはいいいいい\n",cmdBox[0]);
        if(execvp(cmdBox[0], cmdBox) == -1){ //ここでエラー出てる!!!!!!!!!!
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
    return 0;
}

int main(int argc, char *argv[]){
    char* read;
    char* cut;
    char* cutRoom[1024]; 
    char* cmdBox[1024];
    char tmp[1024][64];
    int andLoc;
    int var;
    int branchNum; 
    int builtin_qty;   
    int INcommand;
    int cutCount;
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
    
        
        var = 0;
        INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum,&andLoc,&var);

        printf("%dへへへh\n", branchNum);
        

        if(var == 2){
            (*builtin_func[branchNum])(cmdBox);
            for(i=0; i<(cutCount-andLoc-1); i++){ //①tmpに値を置いておく
                memcpy(tmp[i],cmdBox[andLoc+i+1],12); //&&の後ろをfor文でcopy
            }

            memset(*cmdBox,'\0',1024); //②コマンドボックスを綺麗にする
            for(i=0; i<(cutCount-andLoc-1); i++){
                memcpy(cmdBox[i],tmp[i],64);//③tmpをcmdBoxに戻す
            }



            INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum,&andLoc,&var);
            var = 1; //ここもマジックナンバーなの嫌かも
            if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,var);
            INcommand = 3; //ここも初期値というか決めておくべき
        }


        var = 0; //マジックナンバー
        if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,var);
        memset(*cmdBox, '\0', sizeof(cmdBox));
        memset(*cutRoom, '\0', sizeof(cutRoom));
    }
};