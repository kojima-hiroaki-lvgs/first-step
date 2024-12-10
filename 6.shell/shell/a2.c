#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

int main() {
    char pathname[512];
    char* command = calloc(128,sizeof(char));
    char* tmp_command = calloc(1024, sizeof(char));
    char** complete_command = calloc(128, sizeof(char*));
    for(int i=0; i<128; i++){
        complete_command[i] = calloc(128, sizeof(char));
    }

    while(true){        
        getcwd(pathname, 512);
        printf("%s $ ",pathname);
        fgets(command,1024,stdin);        
        command[strlen(command)-1] = '\0';  
        int i = 0, j = 0, k = 0, space_flag = 0;

        while(command[i] != 0){
            if(command[i] == '|'){
                tmp_command[k] = '\0';
                printf("1\n");
                if(command[i-1] == ' '){
                    complete_command[j] = "|";
                    j++;                    
                } else {                
                    memcpy(complete_command[j],tmp_command,128); 
                    for (int l=0; l<k; l++) tmp_command[l] = 0;                                                
                    complete_command[j+1] = "|"; 
                    j = j+2;     
                }
                k = 0;
            } else if (command[i] == '&' && command[i+1] == '&'){ 
                printf("2\n");           
                if(command[i-1] == ' '){
                    complete_command[j] = "&&";
                    j++;
                } else {                
                    memcpy(complete_command[j],tmp_command,128); 
                    for (int l=0; l<k; l++) tmp_command[l] = 0;                                
                    complete_command[j+1] = "&&";
                    j = j+2;
                }
                i++;
                k = 0;
            } else if (command[i] == '>' && command[i+1] == '>'){
                printf("3\n");
                if(command[i-1] == ' '){
                    complete_command[j] = ">>";
                    j++;
                } else {                
                    memcpy(complete_command[j],tmp_command,128);   
                    for (int l=0; l<k; l++) tmp_command[l] = 0;                                               
                    complete_command[j+1] = ">>";
                    j = j+2;
                }
                i++;
                k = 0;
            } else if (command[i] == '<'){
                printf("4\n");
                if(command[i-1] == ' '){
                    complete_command[j] = "<";  
                    j++;                    
                } else {                
                    memcpy(complete_command[j],tmp_command,128);
                    for (int l=0; l<k; l++) tmp_command[l] = 0;                                                  
                    complete_command[j+1] = "<"; 
                    j = j+2;                   
                }       
                k = 0;
            } else if (command[i] == ' '){
                printf("5\n");
                if(command[i-1] == '&'){
                    j = j - 1;                                
                } else if (command[i-1] == '>'){
                    j = j - 1;
                } else if (command[i-1] == '<'){
                    j = j - 1;
                } else if (command[i-1] == '|'){
                    j = j - 1;
                } else if (command[i-1] == ' '){
                    j = j - 1;
                } else {                                                  
                    memcpy(complete_command[j],tmp_command,128);
                    for (int l=0; l<k; l++) tmp_command[l] = 0;
                }   
                j++;
                space_flag++;
                k = 0; 
            } else {
                tmp_command[k] = command[i];                 
                k++;
            }
            i++;
        }
        complete_command[j] = tmp_command;
        complete_command[j+1] = NULL;

        printf("com: %s\n", complete_command[0]);
        printf("com: %s\n", complete_command[1]);
        printf("com: %s\n", complete_command[2]);
    }

    


    free(command);
    free(tmp_command);
    free(complete_command);
    for(int i=0; i<128; i++){
        free(complete_command[i]);
    }
}
