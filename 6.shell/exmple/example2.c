#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char num[1024] = "101 102 103";
    char a[] = " ";
    char *b;
    char *pointLoc[1024];
    char *tmp;
    // *memset(*pointLoc,0,1024);
    int i = 0;
    
    /*strtokの第1引数は文字列のアドレス, 第2引数は区切り文字群*/
    b = strtok(num, a); //返り値として区切り文字が登場するまでの最初の文字列のポインタを返す
    while (b != NULL){ 
        // printf("%s\n",b);
        pointLoc[i] = b;
        i++;
        b = strtok(NULL, a); //文字列のアドレスをNULLの地点に指定してそこから始める

    }
    printf("%s\n",pointLoc[0]);
    printf("%s\n",pointLoc[1]);
    printf("%s\n",pointLoc[2]);
    printf("%s\n",*pointLoc);

    memcpy(tmp,pointLoc[0],1024);
    printf("%s\n",tmp);

    return 0;

}


//ポインタがずれていくたびに保存する