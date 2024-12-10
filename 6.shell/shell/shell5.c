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
    printf("cd実行\n");
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
    if (cutCount == 1){
        cmdBox[1] = "A";
    }
  
    for (i=0; i<builtin_qty; i++){
        for (k=0; k<cutCount; k++){
            if ((strcmp(cmdBox[0], "cd") == 0) && (cutCount == 1) || strcmp(cmdBox[1], "&&") == 0){ //cdだけの時
                if ((strcmp(cmdBox[1], "&&") == 0) && (strcmp(cmdBox[0], "cd") == 0)){ //cd && ~~　の時
                    chdir("/Users/hiroaki.kojima");
                    *var = 2;//問題
                    *andLoc = 1; //cdの次が&&の時だから&&の位置は1になる
                    printf("&&ここだよ\n");
                    return 2;
                } else if ((strcmp(cmdBox[k],"&&") != 0) && (strcmp(cmdBox[0], "cd") == 0)) { //cdだけの時
                    chdir("/Users/hiroaki.kojima");
                    printf("ここだよ\n");
                    return 1; //ここで0返さないと外部コマンド実行されちゃう
                }
            } else if ((strcmp(cmdBox[0], builtin_str[i]) == 0) && (strcmp(cmdBox[1],"&&") != 0)){ //内部コマンドのリストと合致 & &&が含まれていなかったら  
                printf("&&なし & 内部コマンド\n");
                *branchNum = i;
                *var = 4;
                return (*builtin_func[i])(cmdBox);
            } else if ((strcmp(cmdBox[0], builtin_str[i]) == 0) && (strcmp(cmdBox[k],"&&") == 0)){ //内部リストと合致 & &&が含まれていたら
                printf("&&あり & 内部コマンド\n");
                *andLoc = k;
                *branchNum = i;
                *var = 3;
                return (*builtin_func[i])(cmdBox);
            } else if ((strcmp(cmdBox[0], builtin_str[i]) != 0) && (strcmp(cmdBox[k],"&&") != 0)){ //&&なし & 外部コマンドの場合
                printf("&&なし & 外部コマンド\n");
                return 6;
            } else if ((strcmp(cmdBox[0], builtin_str[i]) != 0) && (strcmp(cmdBox[k],"&&") == 0)){ //&&あり & 外部コマンド
                printf("&&あり & 外部コマンド\n");
                return 5;
            }
        }  
    } 
    printf("例外\n");
    return 0; //内部コマンドないため、外部コマンドを実行    6の場合もあり
}    
   
    


int execute_EXcommand(char *cmdBox[],int cutCount, int var){
    pid_t pid, wpid;
    int status;

    if (cmdBox[1] == NULL){ //cd&&lsの場合、cdが実行されると[ls][][]になる。これだとエラー起きる
        cmdBox[1] = "A";
        printf("通ったよ\n");
    }   
    //ここに&&の場合の処理を書く　外部コマンドから内部コマンドの場合
  
    if (strcmp(cmdBox[1],"&&") == 0 || cutCount == 1){
        cmdBox[1] = NULL;
        printf("通ったよ\n");
    }

     if (var == 0 && strstr(*cmdBox,"&&") != NULL){//初回の時の処理
        cmdBox[cutCount] = NULL; //ここを変数にすることでオプションの数に応じてNULLをおけるようになった
    } else if (var == 1){
        cmdBox[1] = NULL;
        printf("通ったよ\n");
    } 


    pid = fork();
    if (pid == 0){
        printf("実行するコマンドは%s\n",cmdBox[0]);
        if(execvp(cmdBox[0], cmdBox) == -1){ //外部コマンド実行
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

        if(var == 2){//&&がある場合
            (*builtin_func[branchNum])(cmdBox);
            for(i=0; i<(cutCount-andLoc-1); i++){ //①tmpに値を置いておく
                memcpy(tmp[i],cmdBox[andLoc+i+1],12); //&&の後ろをfor文でcopy
            }
            memset(*cmdBox,'\0',1024); //②コマンドボックスを綺麗にする
            for(i=0; i<(cutCount-andLoc-1); i++){
                memcpy(cmdBox[i],tmp[i],64);//③tmpをcmdBoxに戻す
            }
            printf("%sここチェック\n",cmdBox[0]);

            INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum,&andLoc,&var);
            var = 1; //ここもマジックナンバーなの嫌かも
            if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,var);
            //INcommand = 3; //ここも初期値というか決めておくべき
        }

        
        //var = 0; //マジックナンバー
        printf("外部コマンド手前まできたよ\n");
        printf("var1 : %d\n",var);
        printf("INcommand : %d\n",INcommand);
        if(INcommand == 6 || INcommand == 5 ) execute_EXcommand(cmdBox,cutCount,var);
        memset(*cmdBox, '\0', sizeof(cmdBox));
        memset(*cutRoom, '\0', sizeof(cutRoom));
    }
};