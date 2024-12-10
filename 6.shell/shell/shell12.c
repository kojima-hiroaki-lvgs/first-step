#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <wordexp.h>
#include <signal.h>

#define HOME getenv("HOME")

void signalHandler() {
}

char* read_command(){
    char pathname[512];
    char* command = calloc(1024,sizeof(char));
    getcwd(pathname, 512);
    signal(SIGINT,signalHandler);
    printf("%s $ ",pathname);
    fgets(command,1024,stdin);
    if(feof(stdin)){exit(EXIT_SUCCESS);} //ctrl+D押した時に
    command[strlen(command)-1] = '\0';
    return command;
}

char** cut_command(char* command){
    char* tmp_command = calloc(1024, sizeof(char)); 
    char **complete_command = calloc(1024, sizeof(char*));
    for(int i=0; i<512; i++){
        complete_command[i] = calloc(1024, sizeof(char));
    }
    int command_index = 0, cmp_index = 0, tmp_index = 0;
    while(command[command_index] != 0){ //一文字ずつ見ていく
        if(command[command_index] == '|'){ 
            if(command[command_index-1] == ' '){ //前の文字がスペースの場合は文字の追加は行わず、記号だけ追加
                complete_command[cmp_index] = "|";
                cmp_index++;//記号だけの追加なので、1のみ増やす                   
            } else {                
                memcpy(complete_command[cmp_index],tmp_command,1024); //スペースがない場合は記号の前の文字列を入れて、その次に記号を追加する
                for (int i=0; i<tmp_index; i++) tmp_command[i] = 0; //うつし終わったらtmp_commandの中身を0埋めする                                                  
                complete_command[cmp_index+1] = "|"; 
                cmp_index = cmp_index+2; //記号を追加しているので、その分コマンドを飛ばしてあげる     
            }
            tmp_index = 0; //移し終わったらtmpも綺麗にして、0からカウントし直す。
        } else if (command[command_index] == '&' && command[command_index+1] == '&'){ //&&を認識したいため、2つの条件
            if(command[command_index-1] == ' '){
                complete_command[cmp_index] = "&&";
                cmp_index++;
            } else {                
                memcpy(complete_command[cmp_index],tmp_command,1024); 
                for (int i=0; i<tmp_index; i++) tmp_command[i] = 0;                         
                complete_command[cmp_index+1] = "&&";
                cmp_index = cmp_index+2;
            }
            command_index++; //このループの終わりに再度増やすので+2している。記号が1文字ではなく2文字のため
            tmp_index = 0;
        } else if (command[command_index] == '>' && command[command_index+1] == '>'){
            if(command[command_index-1] == ' '){
                complete_command[cmp_index] = ">>";
                cmp_index++;
            } else {                
                memcpy(complete_command[cmp_index],tmp_command,1024);   
                for (int i=0; i<tmp_index; i++) tmp_command[i]=0;                                                
                complete_command[cmp_index+1] = ">>";
                cmp_index = cmp_index+2;
            }
            command_index++;
            tmp_index = 0;
        } else if (command[command_index] == '<'){
            if(command[command_index-1] == ' '){
                complete_command[cmp_index] = "<";  
                cmp_index++;                    
            } else {                
                memcpy(complete_command[cmp_index],tmp_command,1024);
                for (int i=0; i<tmp_index; i++) tmp_command[i] = 0;                                                   
                complete_command[cmp_index+1] = "<"; 
                cmp_index = cmp_index+2;                   
            }       
            tmp_index = 0;
        } else if (command[command_index] == ' '){ //スペースの時の処理
            if(command[command_index-1] == '&'){ //スペースの前が記号の時の処理。-1しないと連続でcompleteに入らず、スペースが入る
                cmp_index = cmp_index - 1;                                
            } else if (command[command_index-1] == '>'){
                cmp_index = cmp_index - 1;
            } else if (command[command_index-1] == '<'){
                cmp_index = cmp_index - 1;
            } else if (command[command_index-1] == '|'){
                cmp_index = cmp_index - 1;
            } else if (command[command_index-1] == ' '){
                cmp_index = cmp_index - 1;
            } else {                                                  
                memcpy(complete_command[cmp_index],tmp_command,1024);//スペースの後ろが文字列の場合、そのままcompleteに移す
                for (int i=0; i<tmp_index; i++) tmp_command[i] = 0; //配列tmp_commandを初期化
            }
            cmp_index++; //completeをずらす
            tmp_index = 0; 
        } else {
            tmp_command[tmp_index] = command[command_index];//記号ではない場合、文字をtmpに1つずつ入れる                 
            tmp_index++; //1つのコマンドごとの文字カウントの役割
        }
        command_index++;//1文字ずらす
    }
    complete_command[cmp_index] = tmp_command; //最後のコマンドをcompleteに入れる
    complete_command[cmp_index+1] = NULL; //最後の配列をNULL埋めして、whileで回す時の終端を作る

    int i = 0, length;
    wordexp_t expandedWords; //wordexp()関数の引数を格納するための構造体
    while(complete_command[i] != NULL){ //コマンドをひとつずつ見ていく
        length = strlen(complete_command[i]);//""用に長さを確保
        if(strcmp(complete_command[i],"$$") == 0){
            char pid_str[20];
            pid_t mypid = getpid();
            snprintf(pid_str, sizeof(pid_str), "%d", mypid); //pidの型を文字列に変換
            complete_command[i] = pid_str; //$$をpidに置き換える
        } else if(complete_command[i][0] == '$'){ //$~で始まるものを認識
            if(wordexp(complete_command[i], &expandedWords, 0) != 0){
                perror("failed to expand");
                
            } //wordexp関数で展開
            complete_command[i] = expandedWords.we_wordv[0]; //展開したものを置き換える                                   
        } else if (complete_command[i][0] == '"' && complete_command[i][length-1] == '"'){
            char command_tmp[256]; 
            for (int j=0; j<length-2; j++){
                command_tmp[j] = complete_command[i][j+1]; //""を除いた部分をtmpに入れる
            }
            complete_command[i] = command_tmp; //tmpで置き換える
        } 
        i++;
    }
    return complete_command;
}

char** redirect_command(char** command_tmp){
    int fd, flag = 0, newCommandIndex = 0;
    char** newCommand_tmp = calloc(1024, sizeof(char*));
    int i = 0;
    while(command_tmp[i] != NULL){ //NULLまで見ていく
        if (strcmp(command_tmp[i],"<") == 0){
            fd = open(command_tmp[i+1], O_RDONLY);
            if(fd == -1){
                perror("failed to open a file"); 
                exit(EXIT_FAILURE);
            }
            if(dup2(fd,0) == -1){
                perror("dup failed for stdin"); //入力をファイルにする
                exit(EXIT_FAILURE);
            }
            if(close(fd) == -1){
                perror ("failed to close fd"); //今後使わないのでclose
                exit(EXIT_FAILURE);
            }
            flag = 1; //次のtxtファイルを飛ばす処理をしたいからフラグ立てる
        } else if (strcmp(command_tmp[i],">") == 0){
            fd = open(command_tmp[i+1], O_WRONLY|O_CREAT|O_TRUNC,0644);
            if (fd == -1){
                perror("failed to open a file"); 
                exit(EXIT_FAILURE);
            }
            if(dup2(fd,1)){
                perror("dup failed for stout"); //入力をファイルにする
                exit(EXIT_FAILURE);
            }
            if(close(fd) == -1){
                perror ("failed to close fd"); //今後使わないのでclose
                exit(EXIT_FAILURE);
            }
            flag = 1;
        } else if (strcmp(command_tmp[i],">>") == 0){
            fd = open(command_tmp[i+1], O_WRONLY|O_CREAT|O_APPEND,0644);
            dup2(fd,1);
            close(fd);
            flag = 1; 
        } else if (flag == 1){ //リダイレクトの次はtxtファイルなので、処理はiだけ増やす
            flag = 0;
        } else { //リダイレクトがない場合
            newCommand_tmp[newCommandIndex] = command_tmp[i]; //なかったらそのままnewCommand_tmpに入っていく
            newCommandIndex++; //newCommandを次にずらす
        }
        i++;
    }
    return newCommand_tmp;
}

void child_process(int pipe_count, char** newCommand_tmp, int pipe_fd[128][2], int max_pipe){
    if (pipe_count != 0){ //pipeがあれば以下のリダイレクトを実行
        max_pipe = max_pipe + 1;
        if (pipe_count == 1){ //初回
            if (dup2(pipe_fd[0][1],1) == -1){  //出力をパイプ1の入口に向ける
                perror("dup failed for output");
                exit(EXIT_FAILURE);
            }
            if (close(pipe_fd[0][1]) == -1){ //出力側のみ閉める(入力は開いていても問題ない)
                perror("failed to close pipe_fd");
                printf("ha\n");
                exit(EXIT_FAILURE);
            }
        } else if (pipe_count < max_pipe){ //二回目から最後の手前まで
            if (dup2(pipe_fd[pipe_count - 2][0],0) == -1){ //入力をパイプ1の出口に向ける
                perror("dup failed for input");
                exit(EXIT_FAILURE);
            }
            if (dup2(pipe_fd[pipe_count - 1][1],1) == -1) { //出力をパイプ2の入口に向ける
                perror("dup failed for output");
                exit(EXIT_FAILURE);
            }
        } else if (pipe_count == max_pipe){ //最後
            if (dup2(pipe_fd[pipe_count - 2][0],0)){ //入力をパイプ1の出口に向ける
                perror("dup failed for input");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (execvp(newCommand_tmp[0],newCommand_tmp) == -1){
        printf("command not found: %s\n",newCommand_tmp[0]); //実行できなかった場合、子プロセス続行されるためエラーを吐かせる
        exit(EXIT_FAILURE);
    }
}

int execute_command(char** command_tmp, int cutRoom_index, int tmp_index, int input_fd, int output_fd, int pipe_fd[128][2], int pipe_count, int max_pipe){
    char** newCommand_tmp = redirect_command(command_tmp); //実行前にリダイレクトを見て入出力を変更
    int status = 0;
    static char cwd[512]; //staticにすると関数が終了しても変数に値が保持される
    static char previous_cwd[512]; 
    
    if(strlen(newCommand_tmp[0]) == 0) {
        //処理なし(入力ない時)
    } else if (strcmp(newCommand_tmp[0],"cd") == 0){ //cdの時
        getcwd(cwd, 512); //cd -用に取得しておく
        if(tmp_index == 1){ //cdだけの場合
            if(chdir(HOME) == -1){
                perror("cd failed");
                exit(EXIT_FAILURE);
            }
        } else if(strcmp(newCommand_tmp[1],"~") == 0){
            if(chdir(HOME) == -1){
                perror("cd failed");
                exit(EXIT_FAILURE);
            }
        } else if(strcmp(newCommand_tmp[1],"-") == 0){ 
            if (chdir(previous_cwd) == -1){
                perror("cd - failed\n");
                exit(EXIT_FAILURE);
            }
        } else {
            if (chdir(newCommand_tmp[cutRoom_index-1]) == -1){ //cdの後にあるディレクトリに移動
                printf("no such file or directory: %s\n", newCommand_tmp[cutRoom_index-1]);
            }
        }
        strcpy(previous_cwd,cwd); //previousにcwdを保持しておいて、次cwdが更新されても前回の値が使えるようにしておく
    } else if(strcmp(newCommand_tmp[0],"exit") == 0){
        exit(EXIT_SUCCESS);
    } else { //外部コマンドの処理
        pid_t pid = fork(); //子プロセスを作る
        if (pid == 0){ //子プロセスでの処理
            child_process(pipe_count,newCommand_tmp, pipe_fd, max_pipe);
        } else if (pid > 0) { //親プロセスでの処理
            if (waitpid(pid, &status, 0) == -1){
                perror("waitpid failed");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("fork error\n");
        }
    }
    return status;
}

int redirect_pipe(char** command_tmp,int input_fd, int output_fd, int pipe_fd[128][2], int pipe_count, int max_pipe){
    char** pipe_command_tmp = calloc(1024, sizeof(char*));
    int pipe_index = 0, pipeTmp_index = 0;
    while(command_tmp[pipe_index] != NULL){ //&&で切り分けた前半の部分を終端まで見ていく(ls -l &&だったら ここでは ls -lが格納されている)
        if (strcmp(command_tmp[pipe_index],"|") == 0){
            pipe_count++; //今ある個数をカウント
            if (pipe_count == 1){ //初回
                if(pipe(pipe_fd[0]) == -1){//パイプ1つ目を作る
                    perror("failed to create pipe");
                    exit(EXIT_FAILURE);
                }
                execute_command(pipe_command_tmp, pipe_index, pipeTmp_index, input_fd, output_fd, pipe_fd, pipe_count, max_pipe);//パイプの前を実行、その先でリダイレクションを制御
                if(close(pipe_fd[0][1]) == -1){//親プロセスでもパイプの入口は閉じておく
                    perror("failed to close pipe_fd");
                    exit(EXIT_FAILURE);
                }
                pipeTmp_index = 0; //パイプ直前までカウントして、パイプがきたら初期化
            } else if (pipe_count > 1){ //2回目以降
                if(pipe(pipe_fd[pipe_count - 1]) == -1){//パイプの数に応じて作成できるようにしてる
                    perror("failed to create pipe");
                    exit(EXIT_FAILURE);
                }
                execute_command(pipe_command_tmp, pipe_index, pipeTmp_index, input_fd, output_fd, pipe_fd, pipe_count, max_pipe);//パイプの前を実行、その先でリダイレクションを制御//実行
                if(close(pipe_fd[pipe_count - 2][0]) == -1){
                    perror("failed to close pipe_fd");
                    exit(EXIT_FAILURE);
                }
                if(close(pipe_fd[pipe_count- 1][1]) == -1){
                    perror("failed to close pipe_fd");
                    exit(EXIT_FAILURE);
                }
                pipeTmp_index = 0;
            }
        } else {
            pipe_command_tmp[pipeTmp_index] = command_tmp[pipe_index];//パイプじゃなかったらtmpにコマンドを格納
            pipeTmp_index++; //tmpをずらす
        }
        pipe_index++;//pipe_indexをずらしてcommand_tmpを次にする
    }
    
    if (pipe_count == 0){
        //パイプがない時は処理なし
    } else {
        pipe_command_tmp[pipeTmp_index] = NULL; //whileでNULLまで回すので入れる
        pipe_count = pipe_count + 1; //pipeの処理は|の個数個 + 終端なので個数+1しないといけない
        execute_command(pipe_command_tmp, pipe_index, pipeTmp_index, input_fd, output_fd, pipe_fd, pipe_count, max_pipe);
        pipeTmp_index = 0;
        if(close(input_fd) == -1){ //これ以上使わないので閉める
            perror("failed to close input_fd");
            exit(EXIT_FAILURE);
        }
        if(close(output_fd) == -1){
            perror("failed to close output_fd");
            exit(EXIT_FAILURE);
        }
    }
    return pipe_count;
}

void process_command(char** complete_command){
    int cutRoom_index = 0, tmp_index = 0;
    int pipe_fd[128][2], pipe_count = 0, pipeTmp_index = 0, max_pipe = 0;
    char** command_tmp = calloc(1024, sizeof(char*));

    int input_fd = dup(0); //事前に標準出入力を3,4にコピーしておく
    int output_fd = dup(1);
    if (input_fd == -1){
        perror("dup failed for input");
        exit(EXIT_FAILURE);
    }
    if (output_fd == -1){
        perror("dup failed for output");
        exit(EXIT_FAILURE);
    }

    while (complete_command[cutRoom_index] != NULL){
        if (strcmp(complete_command[cutRoom_index],"&&") == 0){ //&&を認識してそれ以前のコマンドを切り分ける処理
            command_tmp[tmp_index] = NULL;//command_tmpをwhileで回すために終端にNULLをつける            
            if (redirect_pipe(command_tmp, input_fd, output_fd, pipe_fd, pipe_count, max_pipe) == 0){ //返り値はパイプの数なので、0だったらそのまま&&の後ろのコマンドを実行。あったらパイプでの処理のみ実行
                execute_command(command_tmp, cutRoom_index, tmp_index, input_fd, output_fd, pipe_fd, pipe_count, max_pipe);            
                if (dup2(3,0) == -1){ //execute_commandの中にリダイレクトの処理があり、リダイレクトに入ると標準出入力に戻らないのでここで戻す
                    perror("dup failed for std_input");
                    exit(EXIT_FAILURE);
                }
                if(dup2(4,1) == -1){
                    perror("dup failed for std_output");
                    exit(EXIT_FAILURE);
                }
            }
            tmp_index = 0; //&&までコマンドを入れてカウントしてきたので、新しくカウントするために初期化
            max_pipe = 0; //パイプの処理の後に以下のカウントで追加されないために初期化
        } else {
            if (strcmp(complete_command[cutRoom_index],"|") == 0) max_pipe++; //コマンドに存在するパイプの最大値を事前にカウント(最大値を基準に処理を変える)
            command_tmp[tmp_index] = complete_command[cutRoom_index];//&&がない場合、command_tmpに入れる
            tmp_index++;//入れたら入れる先をずらす
        }
        cutRoom_index++;//complete_commandをずらす           
    }
    command_tmp[tmp_index] = NULL; //上と同様だけど、一番最後のコマンドの処理を以下で実行
    if(redirect_pipe(command_tmp, input_fd, output_fd, pipe_fd, pipe_count, max_pipe) == 0){
        execute_command(command_tmp, cutRoom_index, tmp_index, input_fd, output_fd, pipe_fd, pipe_count, max_pipe);
        if(dup2(3,0) == -1){
            perror("dup failed for std_input");
            exit(EXIT_FAILURE);
        }
        if(dup2(4,1) == -1){
            perror("dup failed for std_input");
            exit(EXIT_FAILURE);
        }
    } 
    tmp_index = 0;
}

void shell(){
    while(true){ //最初に&&を見てコマンドを&&で切り分け、|をみてパイプで切り分け実行、<<>リダイレクトを実行の時に見てあれば実行
        char* command = read_command();
        char** complete_command = cut_command(command);
        process_command(complete_command);
        free(command);
    }
}

int main(){
    shell();
    return 0;
}
    