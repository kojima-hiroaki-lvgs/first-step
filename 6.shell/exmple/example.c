#include <stdio.h>
#include <unistd.h>

int main (){
    char abc = 10;
    char *a;
    a = &abc;
    printf("%s\n",a);
    printf("%d\n",*a);

    char *str1 = "KOJIKOJI";
    char str2 = "KURUKURU";
    str1 = "NEMUI";
    str2 = "ABC";
    printf("%s",str1); //ポインタ変数なら書き換えられる
    printf("%c",str2); 

}

