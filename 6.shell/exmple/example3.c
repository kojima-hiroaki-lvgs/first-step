#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char line[1024];

    while (true) {
        printf("shell> ");
        fgets(line, 1024, stdin);
        printf("%s", line);
        line[strlen(line)-1] = '\0';
        char *argv[] = {line, NULL};
        execvp(argv[0],argv);
    }
}