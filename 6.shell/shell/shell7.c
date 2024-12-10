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
    int i;
    if (strlen(argv[1]) == 0 || strcmp(argv[1],"&&") == 0){
        chdir("/Users/hiroaki.kojima");
    } else {
        chdir(argv[1]); // チェンジディレクトリ
    }
    return 0;
}

int (*builtin_func[])(char **) = { //なんでアドレスとってるんだろう
    &builtin_pwd,
    &builtin_cd,
};

char* read_command(char pathname[512]) {
    char* Read = calloc(1,1024); //ここをmalloc → callocに変更した
    getcwd(pathname, PATHNAME_SIZE);
    printf("%s ~ ☺︎", pathname);
    
    if (fgets(Read,1024,stdin) == NULL){
        if (feof(stdin)) {
            fprintf(stderr,"入力の終了を検出");
            exit(EXIT_SUCCESS);
        } else {
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
            if((strcmp(cmdBox[0], builtin_str[i]) == 0) && (strcmp(cmdBox[k],"&&") == 0)) {
                printf("2 : &&あり & 内部コマンド\n");
                *andLoc = k;
                *branchNum = i;
                *var = 2;
                return 0;
            }
        }  
    }  

    for (i=0; i<builtin_qty; i++){
        for (k=0; k<cutCount; k++){  
        if(strcmp(cmdBox[0], builtin_str[i]) == 0 && strcmp(cmdBox[k],"&&") != 0){
            if((strcmp(cmdBox[0],"cd") == 0) && cutCount == 1){
                goto SKIP3;
            }
            printf("4 : &&なし & 内部コマンド\n");
            printf("cutCount : %d\n",cutCount);
            *branchNum = i;
            *var = 4;
            return (*builtin_func[i])(cmdBox); 
        }
        }          
    }
    
    for (k=0; k<cutCount; k++){        
        if (strcmp(cmdBox[0], builtin_str[0]) != 0 && strcmp(cmdBox[0], builtin_str[1]) != 0 && strcmp(cmdBox[k],"&&") == 0 ){
            printf("5 : &&あり & 外部コマンド\n");
            return 5;
        } 
    }    

    for (k=0; k<cutCount; k++){  
        if (strcmp(cmdBox[0], builtin_str[0]) != 0 && strcmp(cmdBox[0], builtin_str[1]) != 0 && strcmp(cmdBox[k],"&&") != 0){   
            printf("6 : &&なし & 外部コマンド\n");
            printf("cmd : %s\n", cmdBox[0]);
            return 6;
        }   
    }
    
    for (k=0; k<cutCount; k++){
        if ((strcmp(cmdBox[0], "cd") == 0) && (cutCount == 1) || strcmp(cmdBox[1], "&&") == 0){ //cdだけの時
            if ((strcmp(cmdBox[1], "&&") == 0) && (strcmp(cmdBox[0], "cd") == 0)){ //cd && ~~　の時
                chdir("/Users/hiroaki.kojima");
                *var = 2;//外部コマンドへ
                *andLoc = 1; //cdの次が&&の時だから&&の位置は1になる
                return 2;
            } else if ((strcmp(cmdBox[k],"&&") != 0) && (strcmp(cmdBox[0], "cd") == 0)) { //cdだけの時
                SKIP3:
                chdir("/Users/hiroaki.kojima");
                return 1; //ここで0返さないと外部コマンド実行されちゃう
            }
        } 
    }
    printf("例外\n");
    return 0; 
}    

int execute_EXcommand(char *cmdBox[1024],int cutCount, int *var, int *andLoc, int *branchNum,int builtin_qty,int INcommand){
    pid_t pid, wpid;
    int status;
    int andCount;
    int i=0;
    int j=0;
    int k=0;
    int l=0;
    int m=0;
    int x=0;
    char tmp[1024][64];
    char *p;
    char tmp2[1024][64];

    for(k=0; k<cutCount; k++){ //&&の合計
        if(strcmp(cmdBox[k],"&&") == 0){
            i++;
            andCount = i;
        }
    }
    
    for(l=0; l<andCount+1; l++){  

         /*cutCount更新*/    
        if (l != 0 || *var == 2) {
            cutCount = 0;
            x = 0; 
            for(k=0; k<1024; k++){
                if(cmdBox[k] == NULL || strlen(cmdBox[k]) == 0) {
                    break;
                } 
                x++;
            }
            cutCount = x;  
            //printf("更新後cutCount : %d\n",cutCount);
        }
        /*cutCount更新*/

        /*最初から数えて何番目に&&があるか*/
        *andLoc = 0;
        for(k=0; k<cutCount; k++){ //
            if(strcmp(cmdBox[k],"&&") == 0){
                *andLoc = k;
                goto SKIP; //Kを見つけたらforから抜ける(breakだとkが増える)
            }
        }
        SKIP:
        /*最初から数えて何番目に&&があるか*/

        /*条件分け*/
        if (cutCount == 1){//外部コマンド単体 ex)"ls"
            cmdBox[cutCount] = NULL;
            goto EXECUTE;
        }

        if(*var == 4){//内部コマンドの後に&&で外部コマンドが単体でくる場合
            cmdBox[cutCount] = NULL;
        } else if ((cmdBox[1] == NULL) && strcmp(cmdBox[0],"cd") == 0){ //cd&&lsの場合、cdが実行されると[ls][][]になる。これだとエラー起きる
            cmdBox[1] = "A";
        } else if (INcommand != 5 && strcmp(cmdBox[1],"&&") == 0 || cutCount == 1){ //外部コマンドが単体の時(lsのみ)  
            cmdBox[1] = NULL;
        }    

        if(INcommand == 5 || INcommand == 6){ //外部コマンド &&ありなし の場合
                if (*andLoc != 0){
                    cmdBox[*andLoc] = NULL;
                } else {
                    cmdBox[cutCount] = NULL;
                } 
        }
        /*条件分け*/

        /*もし２つ目のコマンドが内部コマンドだった場合の処理*/
        for(i=0; i<builtin_qty; i++){ 
            if(strcmp(cmdBox[0],builtin_str[i]) == 0){
               (*builtin_func[i])(cmdBox);
               goto SKIP2;
            }
        } 
        /*２つ目のコマンドが内部コマンドだった場合の処理*/

        /*実行*/
        EXECUTE:
        pid = fork();
        if (pid == 0){
            printf("1 : 実行するコマンドは%s\n",cmdBox[0]);
            if(execvp(cmdBox[0], cmdBox) == -1){ //外部コマンド実行
                perror("command failed");
            } 
            exit(EXIT_FAILURE);
        } else {
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        /*実行*/

        /*コマンド実行後、これ以上実行するコマンドがない場合*/
        SKIP2:
        if (cutCount == 1){ 
            return 0;
        }
        /*コマンド実行後、これ以上実行するコマンドがない場合*/

        /*コピー*/
        if(cutCount != 1 && INcommand != 6 && *var != 4){//&&がある場合
            for(i=0; i<(cutCount-*andLoc-1); i++){ //①tmpに値を置いておく
                memcpy(tmp[i],cmdBox[*andLoc+i+1],12); //&&の後ろをfor文でcopy
            }
            memset(cmdBox,0,1024); //②コマンドボックスを綺麗にする
            for(i=0; i<(cutCount-*andLoc-1); i++){
                cmdBox[i] = calloc(1,1024);
                memcpy(cmdBox[i],tmp[i],64);//③tmpをcmdBoxに戻す 
                p = strtok(*cmdBox," ");
                while(p!=NULL){
                    j++;
                    p = strtok(NULL," "); 
                }   
            }
            memset(tmp,0,1024);
            cutCount = j;
            cmdBox[cutCount] = NULL;
        }
        /*コピー*/    
    }    
    return 0; //例外の場合は0
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
        memset(cutRoom,0,sizeof(cutRoom));
        builtin_qty = builtin_num();
        read = read_command(pathname);    
        cut = cut_command(read,cutRoom,&cutCount);
       
        memcpy(cmdBox,cutRoom,sizeof(cmdBox));

        
        var = 0;
        INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum,&andLoc,&var);

        REPEAT:
        if(var == 2){//&&がある場合
            if(var == 5 || var == 2){
                (*builtin_func[branchNum])(cmdBox);
            }
            for(i=0; i<(cutCount-andLoc-1); i++){ //①tmpに値を置いておく
                memcpy(tmp[i],cmdBox[andLoc+i+1],12); //&&の後ろをfor文でcopy
            }

            for(i=0; i<1024; i++){
                if ((cmdBox[i] = malloc(1024)) == NULL){
                    printf("失敗\n");
                } 
            }

            memset(*cmdBox,'\0',1024); //②コマンドボックスを綺麗にする
        
            for(i=0; i<(cutCount-andLoc-1); i++){ 
                memcpy(cmdBox[i],tmp[i],64);//③tmpをcmdBoxに戻す
            }

            INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum,&andLoc,&var);
            if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,&var,&andLoc,&branchNum,builtin_qty,INcommand);
            //INcommand = 3; //ここも初期値というか決めておくべき
        }

        if (var == 2){ //内部コマンド3つ以上の時
            goto REPEAT;
        }

        printf("外部コマンド手前まできたよ\n");
        printf("var1 : %d\n",var);
        printf("INcommand : %d\n",INcommand);
        if(INcommand == 6 || INcommand == 5 || var == 3) execute_EXcommand(cmdBox,cutCount,&var,&andLoc,&branchNum,builtin_qty,INcommand);
        memset(cmdBox, 0, sizeof(cmdBox));
        memset(cutRoom, 0, sizeof(cutRoom));
    }
};