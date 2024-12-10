#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>
#include <sys/types.h>  
#include <fcntl.h>
#include <sys/stat.h>

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
    if (strcmp(argv[0],"cd") == 0 && strcmp(argv[1],"A") == 0 || strcmp(argv[0],"cd") == 0 && strcmp(argv[1],"&&") == 0 ){
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
    char *cut2;
    char *tmp;
    char *abc = "kojikoji&&hirohiro";
    char *tmp2[1024];
    int i=0;
    int j;
    int k=0;
    int l;
    char* cutRoom2[1024];
    char* original[1024];
    int next;
   
   for(l=0; l<1024; l++){
    cutRoom[l] = calloc(1,1024);
    cutRoom2[l] = calloc(1,1024);
    original[l] = calloc(1,1024);
    tmp2[l] = calloc(1,1024);
   }

    //空白切り
    cut = strtok(read," "); 
    while (cut != NULL){
        cutRoom[i] = cut;
        tmp = cutRoom[i];
        i++;
        cut = strtok(NULL," ");
    }
    *cutCount = i;

    printf("cutcount : %d\n", *cutCount);
    //空白切り


//空白切りの状態
    for(i=0; i<3; i++){
        memcpy(original[i],cutRoom[i],12);
    }    



//&&で塊を切る
    for(i=0; i<*cutCount; i++){

        printf("aaaa : %s\n", strstr(cutRoom[i],"&&"));
        printf("bbbb : %d\n", i);
        while(strstr(cutRoom[i],"&&") != 0){ //&&が含まれていたら
        //  for(j=0)

            cut2 = strtok(cutRoom[i],"&&");
            while (cut2 != NULL){
                printf("k : %d\n",k);
                // cutRoom2[k] = "&&";
                cutRoom2[k] = cut2;
                tmp = cutRoom2[k];
                printf("cutRoom2111 :%s\n", cutRoom2[k]);
                //k++;
                 k = k+2;
                cut2 = strtok(NULL,"&&");
            }
        }

        printf("cutRoom2 :%s\n", cutRoom2[0]);
        printf("cutRoom2 :%s\n", cutRoom2[1]);
        printf("cutRoom2 :%s\n", cutRoom2[2]);
        printf("cutRoom2 :%s\n", cutRoom2[3]);

        if(strstr(original[i],"&&") == 0){ //&&が含まれないなら
            if(next == 0){
                tmp2[i] = original[i];
                printf("i&&なし : %d\n",i);
            } else if(next == 1){
                tmp2[j] = original[i];
                printf("jjj %d\n",j);
            }    
        } else if (strstr(original[i],"&&") != 0){
            printf("i&&あり : %d\n", i );
            next = 1;
            for (j=0; j<k; j++){ 
                tmp2[i+j] = cutRoom2[j];
            }
        }        
    }
        
        // for(k=0; k<j; k++){
        //     printf("tmp2 : %s\n",tmp2[k]);
        //     printf("%d\n",strcmp(tmp2[2], NULL));
        //     if(strcmp(tmp2[k],0) == 0){
        //         printf("yeah\n");
        //         tmp2[k] = "&&";
        //     }
        // }
        

        printf("tmp2 : %s\n",tmp2[0]);
        printf("tmp2 : %s\n",tmp2[1]);
        printf("tmp2 : %s\n",tmp2[2]);
        printf("tmp2 : %s\n",tmp2[3]);
        printf("tmp2 : %s\n",tmp2[4]);
        




        while(strstr(cutRoom[i],"<") != 0){ //<が含まれていたら
            
        }

        while(strstr(cutRoom[i],">>") != 0){ //>>が含まれていたら
            
        }

        while(strstr(cutRoom[i],"||") != 0){ //>>が含まれていたら
            
        }

       
    

   

    

    

    *cutCount = i; //iが区切りの数&&引数がすでにあるため、cutCountでポインタ渡ししている
    return cutRoom[1024];
}

int execute_INcommand(char *cmdBox[], int builtin_qty,int cutCount,int *branchNum, int *andLoc,int *var, int *extractLoc,int *extractCount){
    pid_t pid, wpid;
    int i, j, k, fd, status, andCount = 0;

    for(i=0; i<cutCount; i++){ //&&の合計
        if(strcmp(cmdBox[i],"&&") == 0){
            j++;
            andCount = j;
        }
    }
   
    if (cutCount == 1){
        cmdBox[1] = "A"; //NULLだとセグフォ起きるから代入
    } 

    for (k=0; k<cutCount; k++){
        if ((strcmp(cmdBox[0], "cd") == 0) && (cutCount == 1) && strcmp(cmdBox[1], "&&") == 0){ //cdだけの時
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

    for (i=0; i<builtin_qty; i++){
        for (k=0; k<cutCount; k++){
            if((strcmp(cmdBox[0], builtin_str[i]) == 0) && (strcmp(cmdBox[k],"&&") == 0)) {
                printf("3 : &&あり & 内部コマンド\n");
                *andLoc = k;
                *branchNum = i;
                *var = 3;
                return 0;
            }
        }  
    }  

    for (i=0; i<builtin_qty; i++){
        for (k=0; k<cutCount; k++){  
            if(strcmp(cmdBox[0], builtin_str[i]) == 0 && strcmp(cmdBox[k],"&&") != 0){
                if((strcmp(cmdBox[0],"cd") == 0) && cutCount == 1){
                    goto SKIP3;
                } else {
                    printf("4 : &&なし & 内部コマンド\n");
                    *branchNum = i;
                    *var = 4;
                    return (*builtin_func[i])(cmdBox); 
                }
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
            return 6;
        }   
    }

    // if (extractCount != 0){ //内部コマンドの>>
    //     pid = fork();
    //     if (pid == 0){
    //         if(*extractCount != 0){
    //             if ((fd = open(cmdBox[*extractLoc+1],O_WRONLY|O_CREAT|O_APPEND,0644)) < 0){
    //                 perror("open error");
    //                 return -1;
    //             }
    //             if ((dup2(fd,1)) < 0){
    //                 perror("dup2 error");
    //                 return -1;
    //             }
    //             close(fd);
    //         }
    //         (*builtin_func[i])(cmdBox);
    //     } else {
    //         do {
    //             wpid = waitpid(pid, &status, WUNTRACED);
    //         } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    //     }
    //     if(strcmp(cmdBox[*extractLoc],">>") == 0 && andCount == 0){
    //         *var = 10;
    //         *extractCount = 0;
    //         *extractLoc = 0;
    //         return 0;
    //     }
    // } 

    printf("例外\n");
    return 0; 
}    

int execute_EXcommand(char *cmdBox[1024],int cutCount, int *var, int *andLoc, int *branchNum,int builtin_qty,int INcommand,int *lessLoc,int *lessCount,int *extractLoc,int *extractCount, int *endPoint, int *endCount){
    pid_t pid, wpid;
    int status;
    int andCount;
    int fd;
    int exist_less;
    int left_qty;
    int i=0;
    int j=0;
    int k=0;
    int l=0;
    char *p;
    char tmp[1024][64];
    char tmp2[1024][64];
    char tmp3[1024][64];
    char buf[256];
    FILE *fp;
    andCount = 0;

    for(i=0; i<cutCount; i++){ //&&の合計
        if(strcmp(cmdBox[i],"&&") == 0){
            j++;
            andCount = j;
        }
    }

        /*cutCount更新*/    
    for(i=0; i<=andCount+1; i++){  
        if (i != 0 || *var == 2) {
            cutCount = 0;
            k = 0; 
            for(j=0; j<1024; j++){
                if(cmdBox[j] == NULL || strlen(cmdBox[j]) == 0) {
                    break;
                } 
                k++;
            }
            cutCount = k;  
        }
         /*cutCount更新*/

        /*lessCount更新*/
        if(*lessCount != 0){
             *lessCount = 0;
            for(j=0; j<cutCount; j++){
                if(strcmp(cmdBox[j],"<") == 0){
                    k++;
                    *lessCount = k;
                    *lessLoc = j;
                }
            }
        }    
        /*lessCount更新*/

        /*extractCount更新*/
        /*extractCount更新*/

        /*最初から数えて何番目に&&があるか*/
        *andLoc = 0;
        for(j=0; j<cutCount; j++){ //
            if(strcmp(cmdBox[j],"&&") == 0){
                *andLoc = j;
                goto SKIP; //Kを見つけたらforから抜ける(breakだとkが増える)
            }
        }
        SKIP:
        /*最初から数えて何番目に&&があるか*/

        /*条件分け*/
        for(j=0; j<builtin_qty; j++){ 
            if (cutCount == 1 && strcmp(cmdBox[0],"cd") != 0 && strcmp(cmdBox[0],"pwd") != 0){//外部コマンド単体 ex)"ls"
                cmdBox[cutCount] = NULL;
                goto EXECUTE;
            }
        } 

        if(*var == 4){//内部コマンドの後に&&で外部コマンドが単体でくる場合
            cmdBox[cutCount] = NULL;
        } else if ((cmdBox[1] == NULL) && strcmp(cmdBox[0],"cd") == 0){ //cd&&lsの場合、cdが実行されると[ls][][]になる。これだとエラー起きる
            cmdBox[1] = "A";
        } else if (*extractCount != 0){
            cmdBox[*extractLoc] = NULL;
        }
        
        /*条件分け*/

        /*もし２つ目のコマンドが内部コマンドだった場合の処理*/
        for(j=0; j<builtin_qty; j++){ 
            if(strcmp(cmdBox[0],builtin_str[j]) == 0){
               (*builtin_func[j])(cmdBox);
               if(cutCount == *endCount){
                    return 0;
               }
                    goto SKIP2;
            }
        } 
        /*２つ目のコマンドが内部コマンドだった場合の処理*/

        /*<を認識して、cat file名にする*/
        if(*lessCount != 0 && strcmp(cmdBox[0],"cat") == 0){
            for(j=0; j<cutCount; j++){
                for(k=0; j<cutCount; k++){ //①tmpに値を置いておく
                    if (k == *lessLoc){ 
                        j = k + 1; //<を飛ばしてその次の要素を入れる
                        for (l=1; l<cutCount-*andLoc; l++){ //&&の後をtmp3へ移す
                            memcpy(tmp3[l-1],cmdBox[*andLoc+l],12);
                            exist_less = l;
                        }
                        memcpy(tmp2[k],cmdBox[j],12);
                        break;
                    } else {
                        memcpy(tmp2[k],cmdBox[k],12); //&&の後ろをfor文でcopy
                    }
                }
                memset(cmdBox,0,1024);
                    for(k=0; k<=cutCount; k++){
                        cmdBox[k] = calloc(1,1024);
                        memcpy(cmdBox[k],tmp2[k],12);
                    }
            /*>を認識して、cat file名にする*/

            /*ファイルを開いて、中身を表示する*/
                fp = fopen(cmdBox[*lessLoc],"r");
                if(fp == NULL){
                    printf("cannot open the file");
                    return -1;
                }
                while(fgets(buf,256,fp) != NULL){ //ファイルの中身を表示
                    printf("%s\n",buf);
                }
                fclose(fp);
            /*ファイルを開いて、中身を表示する*/

            /*&&後のコマンドを実行する用*/
                memset(cmdBox,0,1024); //&&後のコマンドを実行するために、再度memset
                for(j=0; j<cutCount-*andLoc-1; j++){
                    cmdBox[j] = calloc(1,1024);
                    memcpy(cmdBox[j],tmp3[j],12);
                }
                for (j=0; j<builtin_qty; j++){
                    if (strcmp(cmdBox[0], builtin_str[j]) == 0){
                        (*builtin_func[j])(cmdBox); 
                        andCount = andCount - 1;
                    }
                }
                
            /*&&後のコマンドを実行する用*/  
                goto SKIP5;   
            }            
        }

        /*cutCount更新*/    
        if (exist_less != 0 || *var == 2) { //left_qtyは<<を通った場合にしか追加されない
            cutCount = 0;
            k = 0; 
            for(j=0; j<1024; j++){
                if(cmdBox[j] == NULL || strlen(cmdBox[j]) == 0) {
                    break;
                } 
                k++;
            }
            cutCount = k;  
        }
        /*cutCount更新*/

        /*条件分け*/
        if(INcommand == 5 || INcommand == 6 || *var == 3) { //外部コマンド(&&ありなし) 内部コマンド(&&あり) の場合
            if (*andLoc != 0){
                cmdBox[*andLoc] = NULL;
                *var = 7;
            } 
            else if (*var != 9) {
                cmdBox[cutCount] = NULL;
                *var = 8;
            } 
        }

        if(strcmp(cmdBox[0],"<") == 0){ //最後に<来た時に終わる処理
            return 0;
        }
        /*条件分け*/    

        /*実行*/
        EXECUTE:
        pid = fork();
         /*>>を認識する*/
        if (pid == 0){
            if(*extractCount != 0){
                if((fd = open(cmdBox[*extractLoc+1],O_WRONLY|O_CREAT|O_APPEND,0644)) < 0){
                    perror("open error");
                    return -1;
                } 
                if((dup2(fd,1)) < 0){
                    perror("dup2 error");
                    return -1;
                } 
                close(fd);            }
        /*>>を認識する*/
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

        /*NULL埋めした部分をstrcmpでセグフォにならないように&&で埋める*/

        if (*extractCount > 0){
            cmdBox[*extractLoc] = ">>";
        } 
        if (*var == 7){
            cmdBox[*andLoc] = "&&";
        } 
        if (*var == 8){
            cmdBox[cutCount] = "&&";
        }    
        if (strcmp(cmdBox[0],"<") == 0){
            cutCount = cutCount + 1;
        }
        /*NULL埋めした部分をstrcmpで通らないように再度&&で埋める*/    

        /*コマンド実行後、これ以上実行するコマンドがない場合*/

    if (cutCount == *endCount){
        printf("DONE\n");
        return 0;
    }
        /*コマンド実行後、これ以上実行するコマンドがない場合*/

        /*コピー*/
        SKIP2: 
        if(cutCount != 1 && INcommand != 6 && *var != 4){//&&がある場合
            l = 0;
            for(j=0; j<(cutCount-*andLoc-1); j++){ //①tmpに値を置いておく
                memcpy(tmp[j],cmdBox[*andLoc+j+1],12); //&&の後ろをfor文でcopy
                 //printf("tmp : %s\n", tmp[i]);
            }  
            memset(cmdBox,0,1024); //②コマンドボックスを綺麗にする
            for(j=0; j<(cutCount-*andLoc-1); j++){
                cmdBox[j] = calloc(1,1024);
                memcpy(cmdBox[j],tmp[j],64);//③tmpをcmdBoxに戻す 
                p = strtok(*cmdBox," ");
                while(p != NULL){
                    l++;
                    p = strtok(NULL," ");  //ここいる？？？？
                }   
            }
            memset(tmp,0,1024);
            cutCount = 0;
            cutCount = l;
            cmdBox[cutCount] = NULL;     
        }
        /*コピー*/  
        SKIP5:
        continue;
    }    
    return 0; //例外の場合は0
}

int main(int argc, char *argv[]){
    char* read, cut;
    char *cutRoom[1024], *cmdBox[1024];
    char tmp[1024][64];
    int andLoc, var, branchNum, builtin_qty, INcommand, cutCount, lessLoc, lessCount, extractLoc, extractCount, endPoint, endCount, i = 0;
    char pathname[PATHNAME_SIZE]; 
    chdir("/Users/hiroaki.kojima");
    getcwd(pathname, PATHNAME_SIZE);

    // memset(*argv,'\0',1024);
  
    while (true) {
        memset(cutRoom, 0, sizeof(cutRoom));
        builtin_qty = builtin_num();
        read = read_command(pathname);
        cut = *cut_command(read,cutRoom,&cutCount);
       
        memcpy(cmdBox, cutRoom, sizeof(cmdBox));

        for (i=0; i<cutCount; i++) {
            if (strcmp(cmdBox[i],"&&") == 0) {
                endPoint = i;
            }

            else if (strcmp(cmdBox[i],">>") == 0) {  // else 追加
                endPoint = i;
            }

            else if (strcmp(cmdBox[i],"||") == 0) {  // else 追加
                endPoint = i;
            }
        }

        endCount = cutCount - endPoint - 1;
        // printf("endCount :%d\n",endCount);

        /*lessCount代入*/
        for(i=0; i<cutCount; i++){
            if(strcmp(cmdBox[i],"<") == 0){
                lessCount++;
                lessLoc = i;
            }
        }
        /*lessCount代入*/

        /*extractCount代入*/

        for(i=0; i<cutCount; i++){
            if(strcmp(cmdBox[i],">>") == 0){
                extractCount++;
                extractLoc = i;
            }
        }
        /*extractCount代入*/

        
        var = 0;
        INcommand = execute_INcommand(cmdBox, builtin_qty, cutCount, &branchNum, &andLoc, &var, &extractLoc, &extractCount);
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

            INcommand = execute_INcommand(cmdBox,builtin_qty,cutCount,&branchNum,&andLoc,&var,&extractLoc,&extractCount);
            if(INcommand == 1) execute_EXcommand(cmdBox,cutCount,&var,&andLoc,&branchNum,builtin_qty,INcommand,&lessLoc,&lessCount,&extractLoc,&extractCount,&endPoint,&endCount);
            //INcommand = 3; //ここも初期値というか決めておくべき
        } 

        if (var == 2){ //内部コマンド3つ以上の時
            goto REPEAT;
        } else if (var == 10){
            return 0;
        }

        printf("外部コマンド手前まできたよ\n");
        printf("var1 : %d\n",var);
        printf("INcommand : %d\n",INcommand);
        if(INcommand == 6 || INcommand == 5 || var == 3) execute_EXcommand(cmdBox,cutCount,&var,&andLoc,&branchNum,builtin_qty,INcommand,&lessLoc,&lessCount,&extractLoc,&extractCount,&endPoint,&endCount);
        memset(cmdBox, 0, sizeof(cmdBox));
        memset(cutRoom, 0, sizeof(cutRoom));
        
    }

};