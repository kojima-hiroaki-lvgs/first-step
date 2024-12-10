#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    char line[1024];

    while (true) {
        printf("shell> ");
        fgets(line, 1024, stdin);
        printf("%s", line);
    }
}