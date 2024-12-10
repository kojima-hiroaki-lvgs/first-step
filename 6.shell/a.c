#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int status = 0;
    if (fork() == 0) {
        status = 10;
        printf("%d\n", status);
        exit(0);
    } else {
        wait(0);
        status = 1;
        printf("%d\n", status);
    }
    return 0;
}
