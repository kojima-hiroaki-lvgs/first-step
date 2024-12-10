#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

int main() {
    while(true){
        char pathname[512];
        getcwd(pathname, 512);
        printf("%s $ ",pathname);

        char* command = calloc(128,sizeof(char));
        char* new_command = calloc(1024, sizeof(char));
        char** complete_command = calloc(128, sizeof(char*));
        for(int i=0; i<128; i++){
            complete_command[i] = calloc(128, sizeof(char));
        }
        fgets(command,1024,stdin);
        command[strlen(command)-1] = '\0';

  
        int i = 0, j = 0, k = 0, flag = 0;
        while(command[i] != 0){
            if(command[i] == '|'){
                memcpy(complete_command[j],new_command,128);
                j++;
                k = 0;
            } else if (command[i] == '&' && command[i+1] == '&'){
                printf("new : %s\n", new_command);
                if (flag == 1){
                    printf("flag\n");
                    memcpy(complete_command[j],new_command,128);
                    complete_command[j+1] = "&&";
                    flag = 0;
                    j = j + 2;
                    i = i + 1;
                } else {
                    printf("else\n");
                    complete_command[j] = "&&";
                    flag = -1;
                    j = j + 1;
                    i = i + 1;
                }
                k = 0;

            } else if (command[i] == '<' && command[i+1] == '<'){
                memcpy(complete_command[j],new_command,128);
                j++;
                i++;
                k = 0;
            } else if (command[i] == '>'){
                memcpy(complete_command[j],new_command,128);
                j++;
                k = 0;
            } else if (command[i] == ' '){
                if(command[i-1] == '&'){

                } else {
                    memcpy(complete_command[j],new_command,128);
                }                
                j++;
                flag++;
                k = 0;
                
            } else {
                new_command[k] = command[i]; //ここが変わらないのなぜ
                k++;
            }
            i++;
        }

        complete_command[j] = new_command;
   
        printf("com: %s\n", complete_command[0]);
        printf("com: %s\n", complete_command[1]);
        printf("com: %s\n", complete_command[2]);
        printf("com: %s\n", complete_command[3]);
        printf("com: %s\n", complete_command[4]);
        printf("com: %s\n", complete_command[5]);
        printf("com: %s\n", complete_command[6]);


        
        free(command);
    }
}
