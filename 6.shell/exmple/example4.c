#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char test[1024] = "101:こじま 201:くろだ";
    char *pointer;
    char *roomNum[1024];
    char former[1024];
    char latter[1024];
    char *pFormer;
    char *pLatter;
    char *formerRoom[1024];
    char *latterRoom[1024];
    int i = 0;

    pointer = strtok(test, " ");
    while (pointer != NULL){
        roomNum[i] = pointer;
        i++;
        pointer = strtok(NULL, " ");
    }

    // printf("%s\n",roomNum[0]);
    // printf("%s\n",roomNum[1]);
     
    i = 0;  
    strcpy(former,roomNum[0]); // 101:こじま
    pFormer = strtok(former, ":");
    while (pFormer != NULL){
        formerRoom[i] = pFormer;
        i++;
        pFormer = strtok(NULL,":");
    }
    printf("%s\n",formerRoom[0]);
    printf("%s\n",formerRoom[1]);

    i = 0;
    strcpy(latter,roomNum[1]); //102:くろだ
    pLatter = strtok(latter, ":");
    while (pLatter != NULL){
        latterRoom[i] = pLatter;
        i++;
        pLatter = strtok(NULL, ":");
    }
    printf("%s\n",latterRoom[0]);
    printf("%s\n",latterRoom[1]);   


    
 
    return 0;
}