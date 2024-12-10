#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int i;
    char aaa[10]
    aaa[1] = "こんにちは";

    for(i=0; i<10; i++){
        if(!strcmp(aaa[i],"こんにちは")){
            printf("だめー\n");
        }
    }


    return 0;
}